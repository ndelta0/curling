#pragma once

#include <memory>

#include "http_factory.hpp"

namespace curling {
class Context {
	public:
		explicit Context(bool manage_curl = true);
		~Context();

		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&) = delete;
		Context& operator=(Context&&) = delete;

		std::shared_ptr<HttpFactory> CreateFactory();
		std::unique_ptr<HttpClient> CreateClient();
		std::unique_ptr<HttpClient> CreateClient(const Uri& base_uri);

	private:
		bool manages_curl_;
};
}
