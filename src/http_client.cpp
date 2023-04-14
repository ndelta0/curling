#include "http_client.hpp"

#include <curl/curl.h>
#include <optional>
#include <cassert>

#include "version.hpp"
#include "utils.hpp"

curling::HttpClient::~HttpClient() {
	if (easy_handle_ != nullptr) {
		curl_easy_cleanup(easy_handle_);
	}
}

curling::HttpResponseMessage curling::HttpClient::Send(const curling::HttpRequestMessage& request) {
	assert(easy_handle_ != nullptr && "HttpClient is not initialized");

	Uri target;
	if (base_uri_ && !request.uri.IsRelative()) {
		throw std::invalid_argument("URI must be relative");
	} else if (!base_uri_ && request.uri.IsRelative()) {
		throw std::invalid_argument("URI must be absolute");
	} else if (base_uri_) {
		target = *base_uri_ + request.uri;
	} else {
		target = request.uri;
	}
	auto target_str = target.ToString();

	auto result = curl_easy_setopt(easy_handle_, CURLOPT_URL, target_str.c_str());
	if (result != CURLE_OK) {
		return {};
	}

	result = curl_easy_setopt(easy_handle_, CURLOPT_CUSTOMREQUEST, request.method.c_str());
	if (result != CURLE_OK) {
		return {};
	}

	if (request.headers.HasHeader("User-Agent")) {
		result = curl_easy_setopt(easy_handle_, CURLOPT_USERAGENT, (*request.headers.GetHeader("User-Agent")).c_str());
		if (result != CURLE_OK) {
			return {};
		}
	} else {
		result = curl_easy_setopt(easy_handle_, CURLOPT_USERAGENT, "curling/" CURLING_VERSION);
		if (result != CURLE_OK) {
			return {};
		}
	}

	auto [content_type, content_length, content] = request.content != nullptr
	                                               ? request.content->Serialize()
	                                               : std::tuple<std::string, size_t, const std::vector<unsigned char>>{
					"", 0, {}};
	if (content_length > 0) {
		result = curl_easy_setopt(easy_handle_, CURLOPT_POSTFIELDSIZE, content_length);
		if (result != CURLE_OK) {
			return {};
		}
		result = curl_easy_setopt(easy_handle_, CURLOPT_POSTFIELDS, content.data());
		if (result != CURLE_OK) {
			return {};
		}
	} else {
		result = curl_easy_setopt(easy_handle_, CURLOPT_POSTFIELDSIZE, 0);
		if (result != CURLE_OK) {
			return {};
		}
	}

	if (request.headers.HasHeader("Connection") && (*request.headers.GetHeader("Connection")) == "close") {
		result = curl_easy_setopt(easy_handle_, CURLOPT_FORBID_REUSE, 1L);
		if (result != CURLE_OK) {
			return {};
		}
	} else {
		const_cast<HttpRequestHeaders*>(&request.headers)->SetHeader("Connection", "keep-alive");
		result = curl_easy_setopt(easy_handle_, CURLOPT_FORBID_REUSE, 0L);
		if (result != CURLE_OK) {
			return {};
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
		return {};
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
		return {};
	}

	result = curl_easy_setopt(easy_handle_, CURLOPT_WRITEDATA, &response_body);
	if (result != CURLE_OK) {
		curl_slist_free_all(headers);
		return {};
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
		return {};
	}

	result = curl_easy_setopt(easy_handle_, CURLOPT_HEADERDATA, &response_headers);
	if (result != CURLE_OK) {
		curl_slist_free_all(headers);
		return {};
	}

	result = curl_easy_perform(easy_handle_);
	curl_slist_free_all(headers);
	if (result != CURLE_OK) {
		return {};
	}

	long response_code = 0;
	result = curl_easy_getinfo(easy_handle_, CURLINFO_RESPONSE_CODE, &response_code);
	if (result != CURLE_OK) {
		return {};
	}

		HttpResponseMessage	response;
		response.headers_	= response_headers;
	HttpResponseMessage response;
	response.headers_ = response_headers;
	response.status_code_ = (int) response_code;

	HttpContent response_content;
	response_content.data_ = response_body;
	response.content_ = response_content;

	return response;
}

std::optional<std::string> curling::HttpClient::GetString(const curling::Uri& uri) {
	auto response = Get(uri);
	if (response) {
		return response.GetContent().ReadAsString().GetContent();
	}
	return std::nullopt;
}

std::optional<std::string> curling::HttpClient::GetString(const std::string& uri) {
	return GetString(Uri(uri));
}

curling::HttpResponseMessage curling::HttpClient::Get(const curling::Uri& uri) {
	HttpRequestMessage request(HttpMethod::kGet, uri);
	return Send(request);
}

curling::HttpResponseMessage curling::HttpClient::Get(const std::string& uri) {
	return Get(Uri(uri));
}

#ifdef CURLING_HAS_JSON

nlohmann::json curling::HttpClient::GetJson(const curling::Uri& uri) {
	HttpRequestMessage request(HttpMethod::kGet, uri);
	auto response = Send(request);
	if (!response) {
		return nullptr;
	}
	return response.GetContent().ReadAsJson().GetContent();
}

nlohmann::json curling::HttpClient::GetJson(const std::string& uri) {
	return GetJson(Uri(uri));
}

#endif
