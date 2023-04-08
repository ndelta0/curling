#include "http_response_headers.hpp"

std::string curling::HttpResponseHeaders::GetContentType() const {
		if (HasHeader("Content-Type")) {
				return *GetHeader("Content-Type");
		}
		return "";
}

std::string curling::HttpResponseHeaders::GetContentLength() const {
		if (HasHeader("Content-Length")) {
				return *GetHeader("Content-Length");
		}
		return "";
}
