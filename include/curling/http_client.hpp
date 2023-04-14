#pragma once

#include <curl/curl.h>
#include "uri.hpp"
#include "http_response_message.hpp"
#include "http_request_message.hpp"
#include <optional>

#ifdef CURLING_HAS_JSON

#include <nlohmann/json.hpp>

#endif

namespace curling {
class HttpClient {
public:
	~HttpClient();

	HttpResponseMessage Send(const HttpRequestMessage& request);

	HttpResponseMessage Get(const Uri& uri);

	HttpResponseMessage Get(const std::string& uri);

	std::optional<std::string> GetString(const Uri& uri);

	std::optional<std::string> GetString(const std::string& uri);

#ifdef CURLING_HAS_JSON

	nlohmann::json GetJson(const Uri& uri);

	nlohmann::json GetJson(const std::string& uri);

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
