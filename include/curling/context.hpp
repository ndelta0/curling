#pragma once

#include <memory>

#include "http_factory.hpp"

#include "result.hpp"

namespace curling {
class Context {
public:
	explicit Context(bool manage_curl = true);

	~Context();

	Context(const Context&) = delete;

	Context& operator=(const Context&) = delete;

	Context(Context&&) = delete;

	Context& operator=(Context&&) = delete;

	Result<std::shared_ptr<HttpFactory>, Error> CreateFactory();

	Result<std::unique_ptr<HttpClient>, Error> CreateClient();

	Result<std::unique_ptr<HttpClient>, Error> CreateClient(const Uri& base_uri);

private:
	bool manages_curl_;
};
}
