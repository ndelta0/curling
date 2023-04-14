#include "http_factory.hpp"

#include <curl/curl.h>
#include <stdexcept>

#include "http_client.hpp"

curling::HttpFactory::HttpFactory(CURLSH* share_handle) {
	share_handle_ = share_handle;
	if (share_handle_ == nullptr) {
		throw std::runtime_error("Failed to initialize curl share handle");
	}

	auto result = curl_share_setopt(share_handle_, CURLSHOPT_LOCKFUNC,
	                                &HttpFactory::LockCallback);
	if (result != CURLSHE_OK) {
		curl_share_cleanup(share_handle_);
		throw std::runtime_error("Failed to set curl share lock function");
	}

	result = curl_share_setopt(share_handle_, CURLSHOPT_UNLOCKFUNC,
	                           &HttpFactory::UnlockCallback);
	if (result != CURLSHE_OK) {
		curl_share_cleanup(share_handle_);
		throw std::runtime_error("Failed to set curl share unlock function");
	}

	result = curl_share_setopt(share_handle_, CURLSHOPT_USERDATA, this);
	if (result != CURLSHE_OK) {
		curl_share_cleanup(share_handle_);
		throw std::runtime_error("Failed to set curl share user data");
	}

	result = curl_share_setopt(share_handle_, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
	if (result != CURLSHE_OK) {
		curl_share_cleanup(share_handle_);
		throw std::runtime_error("Failed to set curl share DNS lock");
	}

#ifdef CURLING_HAS_SSL
	result = curl_share_setopt(share_handle_, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
	if (result != CURLSHE_OK) {
		curl_share_cleanup(share_handle_);
		throw std::runtime_error("Failed to set curl share SSL session lock");
	}
#endif // CURLING_HAS_SSL

	result = curl_share_setopt(share_handle_, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
	if (result != CURLSHE_OK) {
		curl_share_cleanup(share_handle_);
		throw std::runtime_error("Failed to set curl share connect lock");
	}

	result = curl_share_setopt(share_handle_, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
	if (result != CURLSHE_OK) {
		curl_share_cleanup(share_handle_);
		throw std::runtime_error("Failed to set curl share cookie lock");
	}
}

void curling::HttpFactory::LockCallback(const CURL*, curl_lock_data data,
                                        curl_lock_access access,
                                        void* user_ptr) {
	auto factory = static_cast<HttpFactory*>(user_ptr);

	if (access == CURL_LOCK_ACCESS_SHARED) {
		factory->share_mutexes_[data].lock_shared();
	} else {
		factory->share_mutexes_[data].lock();
	}
}

void curling::HttpFactory::UnlockCallback(const CURL*, curl_lock_data data,
                                          void* user_ptr) {
	auto factory = static_cast<HttpFactory*>(user_ptr);

	factory->share_mutexes_[data].unlock();

	if (factory->share_mutexes_[data].try_lock_shared()) {
		factory->share_mutexes_[data].unlock_shared();
	}
}

curling::HttpFactory::~HttpFactory() {
	if (share_handle_) {
		curl_share_cleanup(share_handle_);
	}
}

std::unique_ptr<curling::HttpClient> curling::HttpFactory::CreateClient() {
	CURL* easy_handle = curl_easy_init();
	if (!easy_handle) {
		return nullptr;
	}

	auto result = curl_easy_setopt(easy_handle, CURLOPT_SHARE, share_handle_);
	if (result != CURLE_OK) {
		curl_easy_cleanup(easy_handle);
		return nullptr;
	}

	auto client = new HttpClient(easy_handle);
	return std::unique_ptr<HttpClient>(client);
}

std::unique_ptr<curling::HttpClient>
curling::HttpFactory::CreateClient(const curling::Uri& base_uri) {
	if (base_uri.IsRelative()) {
		throw std::invalid_argument("Base URI must be absolute");
	}

	auto client = CreateClient();
	if (!client) {
		return nullptr;
	}

	client->base_uri_ = base_uri;

	return client;
}
