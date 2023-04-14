#include "http_response_message.hpp"

#include <stdexcept>

using namespace curling;

const HttpContent& curling::HttpResponseMessage::GetContent() {
	return content_;
}

const HttpResponseHeaders& HttpResponseMessage::GetHeaders() {
	return headers_;
}

int HttpResponseMessage::GetStatusCode() const {
	return status_code_;
}

bool HttpResponseMessage::IsSuccessStatusCode() const {
	return status_code_ >= 200 && status_code_ < 300;
}

void HttpResponseMessage::EnsureSuccessStatusCode() const {
	if (!IsSuccessStatusCode()) {
		throw std::runtime_error("HTTP request failed with status code: " + std::to_string(status_code_));
	}
}
