#include "context.hpp"

#include <memory>
#include <curl/curl.h>

#include "http_factory.hpp"

using namespace curling;

curling::Context::Context(bool manage_curl) {
	manages_curl_ = manage_curl;
	if (manages_curl_) {
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}
}

curling::Context::~Context() {
	if (manages_curl_) {
		curl_global_cleanup();
	}
}

std::shared_ptr<HttpFactory>
curling::Context::CreateFactory() { // NOLINT(readability-convert-member-functions-to-static)
	CURLSH* share_handle = nullptr;
	try {
		share_handle = curl_share_init();
		if (share_handle == nullptr) {
			return nullptr;
		}

		auto* factory = new HttpFactory(share_handle);
		return std::shared_ptr<HttpFactory>(factory);
	} catch (...) {
		if (share_handle != nullptr) {
			curl_share_cleanup(share_handle);
		}
		return nullptr;
	}
}

std::unique_ptr<HttpClient> Context::CreateClient() { // NOLINT(readability-convert-member-functions-to-static)
	CURL* easy_handle = nullptr;
	try {
		easy_handle = curl_easy_init();
		if (easy_handle == nullptr) {
			return nullptr;
		}

		auto* client = new HttpClient(easy_handle);
		return std::unique_ptr<HttpClient>(client);
	} catch (...) {
		if (easy_handle != nullptr) {
			curl_easy_cleanup(easy_handle);
		}
		return nullptr;
	}
}

std::unique_ptr<HttpClient> Context::CreateClient(const Uri& base_uri) {
	if (base_uri.IsRelative()) {
		return nullptr;
	}

	auto client = CreateClient();
	if (client == nullptr) {
		return nullptr;
	}

	client->base_uri_ = base_uri;
	return client;
}
