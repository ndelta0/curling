#pragma once

#include <curl/curl.h>
#include "uri.hpp"
#include "http_response_message.hpp"
#include "http_request_message.hpp"

#include "result.hpp"

#ifdef CURLING_HAS_JSON

#include <nlohmann/json.hpp>

#endif

namespace curling {
class HttpClient {
public:
	~HttpClient();

	Result<HttpResponseMessage, Error> Send(const HttpRequestMessage& request);

	Result<HttpResponseMessage, Error> Get(const Uri& uri);

	Result<HttpResponseMessage, Error> Get(const std::string& uri);

	Result<std::string, Error> GetString(const Uri& uri);

	Result<std::string, Error> GetString(const std::string& uri);

#ifdef CURLING_HAS_JSON

	Result<nlohmann::json, Error> GetJson(const Uri& uri);

	Result<nlohmann::json, Error> GetJson(const std::string& uri);

#endif

private:
	friend class HttpFactory;

	friend class Context;

	explicit HttpClient(CURL* easy_handle) {
		if (easy_handle == nullptr) {
			throw std::runtime_error("easy_handle is null");
		}
		easy_handle_ = easy_handle;
	}

	CURL* easy_handle_;

	std::optional<Uri> base_uri_;
};
}
