#include "http_request_message.hpp"

std::string MethodToString(curling::HttpMethod method) {
		using namespace curling;
		switch (method) {
				case HttpMethod::kGet:
						return "GET";
				case HttpMethod::kPost:
						return "POST";
				case HttpMethod::kPut:
						return "PUT";
				case HttpMethod::kDelete:
						return "DELETE";
				case HttpMethod::kHead:
						return "HEAD";
				case HttpMethod::kPatch:
						return "PATCH";
				default:
						throw std::runtime_error("Unknown HttpMethod");
		}
}

curling::HttpRequestMessage::HttpRequestMessage(curling::HttpMethod method, const std::string& uri) {
		this->method = MethodToString(method);
		this->uri = Uri(uri);
}

curling::HttpRequestMessage::HttpRequestMessage(curling::HttpMethod method, const curling::Uri& uri) {
		this->method = MethodToString(method);
		this->uri = uri;
}
