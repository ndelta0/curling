#include "http_client.hpp"

#include <curl/curl.h>
#include <optional>
#include <cassert>

#include "version.hpp"
#include "utils.hpp"
#include "tracing.hpp"

curling::HttpClient::~HttpClient() {
	if (easy_handle_ != nullptr) {
		curl_easy_cleanup(easy_handle_);
	}
}

curling::Result<curling::HttpResponseMessage, curling::Error> curling::HttpClient::Send(const curling::HttpRequestMessage& request) {
	assert(easy_handle_ != nullptr && "HttpClient is not initialized");

	auto tracer = tracing::GetTracer();
	auto span = tracer->StartSpan("Curling Send");
	[[maybe_unused]] auto scope = tracer->WithActiveSpan(span);

	Uri target;
	if (!base_uri_ && !request.uri.IsAbsolute()) {
		span->SetStatus(opentelemetry::trace::StatusCode::kError, "URI must be absolute");
		return Error::kUriMustBeAbsolute;
	} else if (base_uri_ && request.uri.IsRelative()) {
		target = *base_uri_ + request.uri;
	} else {
		target = request.uri;
	}
	auto target_str = target.ToString();

	span->SetAttribute("http.authority", target.GetAuthority());
	span->SetAttribute("http.method", request.method);
	span->SetAttribute("http.url", target_str);

	auto result = curl_easy_setopt(easy_handle_, CURLOPT_URL, target_str.c_str());
	if (result != CURLE_OK) {
		span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set URL");
		return Error::kCurlInternalError;
	}

	result = curl_easy_setopt(easy_handle_, CURLOPT_CUSTOMREQUEST, request.method.c_str());
	if (result != CURLE_OK) {
		span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set method");
		return Error::kCurlInternalError;
	}

	if (request.headers.HasHeader("User-Agent")) {
		result = curl_easy_setopt(easy_handle_, CURLOPT_USERAGENT, (*request.headers.GetHeader("User-Agent")).c_str());
		if (result != CURLE_OK) {
			span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set user agent");
			return Error::kCurlInternalError;
		}
	} else {
		result = curl_easy_setopt(easy_handle_, CURLOPT_USERAGENT, "curling/" CURLING_VERSION);
		if (result != CURLE_OK) {
			span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set user agent");
			return Error::kCurlInternalError;
		}
	}

	auto [content_type, content_length, content] = request.content != nullptr
	                                               ? request.content->Serialize()
	                                               : std::tuple<std::string, size_t, const std::vector<unsigned char>>{
					"", 0, {}};
	if (content_length > 0) {
		result = curl_easy_setopt(easy_handle_, CURLOPT_POSTFIELDSIZE, content_length);
		if (result != CURLE_OK) {
			span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set content length");
			return Error::kCurlInternalError;
		}
		result = curl_easy_setopt(easy_handle_, CURLOPT_POSTFIELDS, content.data());
		if (result != CURLE_OK) {
			span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set content");
			return Error::kCurlInternalError;
		}
	} else {
		result = curl_easy_setopt(easy_handle_, CURLOPT_POSTFIELDSIZE, 0);
		if (result != CURLE_OK) {
			span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set content length");
			return Error::kCurlInternalError;
		}
	}

	if (request.headers.HasHeader("Connection") && (*request.headers.GetHeader("Connection")) == "close") {
		result = curl_easy_setopt(easy_handle_, CURLOPT_FORBID_REUSE, 1L);
		if (result != CURLE_OK) {
			span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set connection");
			return Error::kCurlInternalError;
		}
	} else {
		const_cast<HttpRequestHeaders*>(&request.headers)->SetHeader("Connection", "keep-alive");
		result = curl_easy_setopt(easy_handle_, CURLOPT_FORBID_REUSE, 0L);
		if (result != CURLE_OK) {
			span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set connection");
			return Error::kCurlInternalError;
		}
	}

	curl_slist* headers = nullptr;
	for (const auto& [key, value]: request.headers.GetHeaders()) {
		if (key == "User-Agent" || key == "Content-Type" || key == "Content-Length") {
			continue;
		}
		auto header = key;
		header += ": " + value;
		headers = curl_slist_append(headers, header.c_str());
	}
	if (content_length > 0) {
		auto header = "Content-Type: " + content_type;
		headers = curl_slist_append(headers, header.c_str());
		header = "Content-Length: " + std::to_string(content_length);
		headers = curl_slist_append(headers, header.c_str());
	}
	result = curl_easy_setopt(easy_handle_, CURLOPT_HTTPHEADER, headers);
	if (result != CURLE_OK) {
		curl_slist_free_all(headers);
		span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set headers");
		return Error::kCurlInternalError;
	}

	std::vector<unsigned char> response_body;
	result = curl_easy_setopt(easy_handle_, CURLOPT_WRITEFUNCTION, +[](char* data,
	                                                                   size_t count, size_t unit_size,
	                                                                   void* user_data) {
		auto& buffer = *static_cast<std::vector<unsigned char>*>(user_data);
		auto size = count * unit_size;
		buffer.insert(buffer.end(), data, data + size);
		return size;
	});
	if (result != CURLE_OK) {
		curl_slist_free_all(headers);
		span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set write function");
		return Error::kCurlInternalError;
	}

	result = curl_easy_setopt(easy_handle_, CURLOPT_WRITEDATA, &response_body);
	if (result != CURLE_OK) {
		curl_slist_free_all(headers);
		span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set write data");
		return Error::kCurlInternalError;
	}

	HttpResponseHeaders response_headers;
	result = curl_easy_setopt(easy_handle_, CURLOPT_HEADERFUNCTION, +[](char* data,
	                                                                    size_t count, size_t unit_size,
	                                                                    void* user_data) {
		auto& headers = *static_cast<HttpResponseHeaders*>(user_data);
		auto size = count * unit_size;
		auto header = std::string(data, data + size);
		auto pos = header.find(':');
		if (pos != std::string::npos) {
			auto key = Utils::Trim(header.substr(0, pos), " \t\r\n");
			auto value = Utils::Trim(header.substr(pos + 1), " \t\r\n");
			headers.SetHeader(key, value);
		}
		return size;
	});
	if (result != CURLE_OK) {
		curl_slist_free_all(headers);
		span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set header function");
		return Error::kCurlInternalError;
	}

	result = curl_easy_setopt(easy_handle_, CURLOPT_HEADERDATA, &response_headers);
	if (result != CURLE_OK) {
		curl_slist_free_all(headers);
		span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to set header data");
		return Error::kCurlInternalError;
	}

	result = curl_easy_perform(easy_handle_);
	curl_slist_free_all(headers);
	if (result != CURLE_OK) {
		span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to perform request");

		switch (result) {
			case CURLE_COULDNT_RESOLVE_PROXY:
			case CURLE_COULDNT_RESOLVE_HOST:
				span->SetAttribute("curl.error", "Could not resolve host");
				span->SetAttribute("curl.error_details", curl_easy_strerror(result));
				span->SetAttribute("curl.error_code", std::to_string(result));
				return Error::kCouldNotResolveHost;
			case CURLE_COULDNT_CONNECT:
				span->SetAttribute("curl.error", "Could not connect");
				span->SetAttribute("curl.error_details", curl_easy_strerror(result));
				span->SetAttribute("curl.error_code", std::to_string(result));
				return Error::kCouldNotConnect;
			case CURLE_UNSUPPORTED_PROTOCOL:
			case CURLE_URL_MALFORMAT:
			case CURLE_HTTP_RETURNED_ERROR:
			case CURLE_OPERATION_TIMEDOUT:
			case CURLE_RANGE_ERROR:
			case CURLE_HTTP_POST_ERROR:
			case CURLE_TOO_MANY_REDIRECTS:
				span->SetAttribute("curl.error", "HTTP error");
				span->SetAttribute("curl.error_details", curl_easy_strerror(result));
				span->SetAttribute("curl.error_code", std::to_string(result));
				return Error::kHttpError;
			case CURLE_ABORTED_BY_CALLBACK:
				span->SetAttribute("curl.error", "Aborted");
				span->SetAttribute("curl.error_details", curl_easy_strerror(result));
				span->SetAttribute("curl.error_code", std::to_string(result));
				return Error::kAborted;
			case CURLE_SSL_ENGINE_NOTFOUND:
			case CURLE_SSL_ENGINE_SETFAILED:
			case CURLE_SSL_CERTPROBLEM:
			case CURLE_SSL_CIPHER:
			case CURLE_USE_SSL_FAILED:
			case CURLE_SSL_ENGINE_INITFAILED:
			case CURLE_SSL_CACERT_BADFILE:
			case CURLE_SSL_SHUTDOWN_FAILED:
			case CURLE_SSL_CRL_BADFILE:
			case CURLE_SSL_ISSUER_ERROR:
			case CURLE_SSL_PINNEDPUBKEYNOTMATCH:
			case CURLE_SSL_INVALIDCERTSTATUS:
			case CURLE_SSL_CLIENTCERT:
				span->SetAttribute("curl.error", "SSL error");
				span->SetAttribute("curl.error_details", curl_easy_strerror(result));
				span->SetAttribute("curl.error_code", std::to_string(result));
				return Error::kCurlSslError;
			default:
				span->SetAttribute("curl.error", "Unknown error");
				span->SetAttribute("curl.error_details", curl_easy_strerror(result));
				span->SetAttribute("curl.error_code", std::to_string(result));
				return Error::kCurlRequestFailed;
		}
	}

	long response_code = 0;
	result = curl_easy_getinfo(easy_handle_, CURLINFO_RESPONSE_CODE, &response_code);
	if (result != CURLE_OK) {
		span->SetStatus(opentelemetry::trace::StatusCode::kError, "Failed to get response code");
		return Error::kCurlInternalError;
	}

	span->SetAttribute("http.status_code", std::to_string(response_code));

	HttpResponseMessage response;
	response.headers_ = response_headers;
	response.status_code_ = (int) response_code;

	HttpContent response_content;
	response_content.data_ = response_body;
	response.content_ = response_content;

	return response;
}

curling::Result<std::string, curling::Error> curling::HttpClient::GetString(const curling::Uri& uri) {
	auto response = Get(uri);
	if (response) {
		return response->GetContent().ReadAsString().GetContent();
	}
	return *response.Error();
}

curling::Result<std::string, curling::Error> curling::HttpClient::GetString(const std::string& uri) {
	return GetString(Uri(uri));
}

curling::Result<curling::HttpResponseMessage, curling::Error> curling::HttpClient::Get(const curling::Uri& uri) {
	HttpRequestMessage request(HttpMethod::kGet, uri);
	return Send(request);
}

curling::Result<curling::HttpResponseMessage, curling::Error> curling::HttpClient::Get(const std::string& uri) {
	return Get(Uri(uri));
}

#ifdef CURLING_HAS_JSON

curling::Result<nlohmann::json, curling::Error> curling::HttpClient::GetJson(const curling::Uri& uri) {
	HttpRequestMessage request(HttpMethod::kGet, uri);
	auto response = Send(request);
	if (!response) {
		return *response.Error();
	}
	return response->GetContent().ReadAsJson().GetContent();
}

curling::Result<nlohmann::json, curling::Error> curling::HttpClient::GetJson(const std::string& uri) {
	return GetJson(Uri(uri));
}

#endif
