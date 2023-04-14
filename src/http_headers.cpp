#include "http_headers.hpp"

#include <algorithm>

std::string curling::HttpHeaders::NormalizeKey(const std::string& key) {
	std::string normalized_key = key;
	std::transform(normalized_key.begin(), normalized_key.end(), normalized_key.begin(), ::tolower);
	return normalized_key;
}

void curling::HttpHeaders::SetHeader(const std::string& key, const std::string& value) {
	auto normalized_key = NormalizeKey(key);
	headers_[normalized_key] = value;
}

void curling::HttpHeaders::RemoveHeader(const std::string& key) {
	auto normalized_key = NormalizeKey(key);
	headers_.erase(normalized_key);
}

void curling::HttpHeaders::ClearHeaders() {
	headers_.clear();
}

const std::map<std::string, std::string>& curling::HttpHeaders::GetHeaders() const {
	return headers_;
}

std::optional<const std::string> curling::HttpHeaders::GetHeader(const std::string& key) const {
	if (HasHeader(key)) {
		auto normalized_key = NormalizeKey(key);
		return headers_.at(normalized_key);
	}
	return std::nullopt;
}

bool curling::HttpHeaders::HasHeader(const std::string& key) const {
	auto normalized_key = NormalizeKey(key);
	return headers_.find(normalized_key) != headers_.end();
}
