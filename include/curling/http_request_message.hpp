#pragma once

#include <string>
#include "uri.hpp"
#include "http_content.hpp"
#include "http_request_headers.hpp"

namespace curling {
enum class HttpMethod {
	kGet,
	kPost,
	kPut,
	kPatch,
	kDelete,
	kHead
};

struct HttpRequestMessage {
	HttpRequestMessage() = default;

	HttpRequestMessage(HttpMethod method, const std::string& uri);

	HttpRequestMessage(HttpMethod method, const Uri& uri);

	~HttpRequestMessage() = default;

	std::string method;
	Uri uri;
	std::shared_ptr<HttpContent> content;
	HttpRequestHeaders headers;
};
}
