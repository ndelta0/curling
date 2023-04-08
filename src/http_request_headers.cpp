#include "http_request_headers.hpp"

void curling::HttpRequestHeaders::SetAccept(const std::string& accept) {
		SetHeader("Accept", accept);
}

void curling::HttpRequestHeaders::SetAuthorization(const std::string& authorization) {
		SetHeader("Authorization", authorization);
}

void curling::HttpRequestHeaders::SetContentType(const std::string& content_type) {
		SetHeader("Content-Type", content_type);
}

void curling::HttpRequestHeaders::SetContentLength(const std::string& content_length) {
		SetHeader("Content-Length", content_length);
}

void curling::HttpRequestHeaders::SetUserAgent(const std::string& user_agent) {
		SetHeader("User-Agent", user_agent);
}
