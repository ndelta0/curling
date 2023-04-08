#pragma once

#include <curl/curl.h>
#include <unordered_map>
#include <shared_mutex>
#include <memory>

#include "http_client.hpp"
#include "uri.hpp"

namespace curling {
class HttpFactory {
	public:
		~HttpFactory();

		std::unique_ptr<HttpClient> CreateClient();
		std::unique_ptr<HttpClient> CreateClient(const Uri& base_uri);

	private:
		friend class Context;
		explicit HttpFactory(CURLSH* share_handle);

		CURLSH* share_handle_;
		std::unordered_map<curl_lock_data, std::shared_mutex> share_mutexes_;

		static void
		LockCallback(const CURL*, curl_lock_data data, curl_lock_access access,
		             void* user_ptr);

		static void UnlockCallback(const CURL*, curl_lock_data data, void* user_ptr);
};
}
