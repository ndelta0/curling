#pragma once

#include "http_headers.hpp"

namespace curling {
class HttpResponseHeaders : public HttpHeaders {
	public:
		HttpResponseHeaders() = default;

		~HttpResponseHeaders() = default;

		[[nodiscard]] std::string GetContentType() const;

		[[nodiscard]] std::string GetContentLength() const;
};
}
