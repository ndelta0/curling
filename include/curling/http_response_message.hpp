#pragma once

#include "http_response_headers.hpp"
#include "http_content.hpp"

namespace curling {
class HttpResponseMessage {
	public:
		HttpResponseMessage() = default;

		~HttpResponseMessage() = default;

		[[nodiscard]] const HttpContent& GetContent();

		[[nodiscard]] const HttpResponseHeaders& GetHeaders();

		[[nodiscard]] int GetStatusCode() const;

		[[nodiscard]] bool IsSuccessStatusCode() const;

		void EnsureSuccessStatusCode() const;

		// Operator bool
		[[nodiscard]] explicit operator bool() const {
				return IsSuccessStatusCode();
		}

		bool operator!() const {
				return !IsSuccessStatusCode();
		}

	private:
		friend class HttpClient;

		HttpContent content_;
		HttpResponseHeaders headers_;
		int status_code_ = 0;
};
}
