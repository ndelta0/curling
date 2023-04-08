#include <stdexcept>
#include "http_content.hpp"
#include "utils.hpp"

curling::content::ByteArrayContent curling::HttpContent::ReadAsByteArray() const {
		return {data_};
}

curling::content::StringContent curling::HttpContent::ReadAsString() const {
		return {std::string(data_.begin(), data_.end())};
}

curling::content::FormUrlEncodedContent curling::HttpContent::ReadAsFormUrlEncoded() const {
		return content::FormUrlEncodedContent::Parse(std::string(data_.begin(), data_.end()));
}

curling::content::JsonContent curling::HttpContent::ReadAsJson() const {
		return {nlohmann::json::parse(std::string(data_.begin(), data_.end()))};
}

std::tuple<std::string, size_t, const std::vector<unsigned char>> curling::HttpContent::Serialize() const {
		return {"application/octet-stream", data_.size(), data_};
}

curling::content::ByteArrayContent::ByteArrayContent(const std::vector<unsigned char>& content) {
		data_ = content;
}

std::tuple<std::string, size_t, const std::vector<unsigned char>>
curling::content::ByteArrayContent::Serialize() const {
		return {"application/octet-stream", data_.size(), data_};
}

void curling::content::ByteArrayContent::SetContent(const std::vector<unsigned char>& content) {
		data_ = content;
}

const std::vector<unsigned char>& curling::content::ByteArrayContent::GetContent() const {
		return data_;
}

size_t curling::content::ByteArrayContent::GetSize() const {
		return data_.size();
}

curling::content::StringContent::StringContent(const std::string& content) {
		data_ = content;
}

curling::content::StringContent::StringContent(const std::string& content, const std::string& content_type) {
		data_ = content;
		content_type_ = content_type;
}

std::tuple<std::string, size_t, const std::vector<unsigned char>> curling::content::StringContent::Serialize() const {
		return {content_type_, data_.size(), {data_.begin(), data_.end()}};
}

void curling::content::StringContent::SetContent(const std::string& content) {
		data_ = content;
}

void curling::content::StringContent::SetContentType(const std::string& content_type) {
		content_type_ = content_type;
}

const std::string& curling::content::StringContent::GetContent() const {
		return data_;
}

const std::string& curling::content::StringContent::GetContentType() const {
		return content_type_;
}

size_t curling::content::StringContent::GetSize() const {
		return data_.size();
}

curling::content::FormUrlEncodedContent::FormUrlEncodedContent(
								const std::vector<std::pair<std::string, std::string>>& content) {
		data_ = content;
}

curling::content::FormUrlEncodedContent
curling::content::FormUrlEncodedContent::Parse(const std::string& content) {
		std::vector<std::pair<std::string, std::string>> data;

		auto query_pairs = Utils::Split(content, '&');
		for (const auto& pair: query_pairs) {
				auto key_value = Utils::Split(pair, '=');

				if (key_value.size() == 1) {
						data.emplace_back(Utils::UrlDecode(key_value[0]), "");
				} else if (key_value.size() == 2) {
						if (key_value[0].empty()) {
								throw std::invalid_argument("Invalid query");
						} else {
								data.emplace_back(Utils::UrlDecode(key_value[0]), Utils::UrlDecode(key_value[1]));
						}
				} else {
						throw std::invalid_argument("Invalid query");
				}
		}

		return {data};
}

std::tuple<std::string, size_t, const std::vector<unsigned char>>
curling::content::FormUrlEncodedContent::Serialize() const {
		std::string content;
		for (const auto& pair: data_) {
				if (!content.empty()) {
						content += "&";
				}

				content += Utils::UrlEncode(pair.first) + "=" + Utils::UrlEncode(pair.second);
		}

		return {"application/x-www-form-urlencoded", content.size(), {content.begin(), content.end()}};
}

void
curling::content::FormUrlEncodedContent::SetContent(const std::vector<std::pair<std::string, std::string>>& content) {
		data_ = content;
}

void curling::content::FormUrlEncodedContent::AddContent(const std::string& key, const std::string& value) {
		data_.emplace_back(key, value);
}

const std::vector<std::pair<std::string, std::string>>& curling::content::FormUrlEncodedContent::GetContent() const {
		return data_;
}

size_t curling::content::FormUrlEncodedContent::GetSize() const {
		return data_.size();
}

#ifdef CURLING_HAS_JSON

curling::content::JsonContent::JsonContent(const nlohmann::json& content) {
		data_ = content;
}

std::tuple<std::string, size_t, const std::vector<unsigned char>> curling::content::JsonContent::Serialize() const {
		auto dump = data_.dump();
		return {"application/json", dump.size(), {dump.begin(), dump.end()}};
}

void curling::content::JsonContent::SetContent(const nlohmann::json& content) {
		data_ = content;
}

const nlohmann::json& curling::content::JsonContent::GetContent() const {
		return data_;
}

#endif
