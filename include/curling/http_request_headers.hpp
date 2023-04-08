#pragma once

#include "http_headers.hpp"

namespace curling {
class HttpRequestHeaders : public HttpHeaders {
	public:
		HttpRequestHeaders() = default;

		~HttpRequestHeaders() = default;

		void SetAccept(const std::string& accept);

		void SetAuthorization(const std::string& authorization);

		void SetContentType(const std::string& content_type);

		void SetContentLength(const std::string& content_length);

		void SetUserAgent(const std::string& user_agent);
};
}
