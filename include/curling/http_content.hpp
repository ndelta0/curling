#pragma once

#include <vector>
#include <tuple>
#include <string>
#include <cstddef>

#ifdef CURLING_HAS_JSON

#include <nlohmann/json.hpp>

#endif

namespace curling {
namespace content { // Forward declarations
class ByteArrayContent;

class StringContent;

class FormUrlEncodedContent;

#ifdef CURLING_HAS_JSON

class JsonContent;

#endif
}

class HttpContent {
	public:
		HttpContent() = default;

		~HttpContent() = default;

		[[nodiscard]] virtual std::tuple<std::string, size_t, const std::vector<unsigned char>> Serialize() const;

		[[nodiscard]] content::ByteArrayContent ReadAsByteArray() const;

		[[nodiscard]] content::StringContent ReadAsString() const;

		[[nodiscard]] content::FormUrlEncodedContent ReadAsFormUrlEncoded() const;

#ifdef CURLING_HAS_JSON
		[[nodiscard]] content::JsonContent ReadAsJson() const;
#endif

	protected:
		friend class HttpClient;

		std::vector<unsigned char> data_;
};

namespace content {
class ByteArrayContent : public HttpContent {
	public:
		ByteArrayContent() = default;

		ByteArrayContent(const std::vector<unsigned char>& content); // NOLINT(google-explicit-constructor)

		~ByteArrayContent() = default;

		[[nodiscard]] std::tuple<std::string, size_t, const std::vector<unsigned char>> Serialize() const override;

		void SetContent(const std::vector<unsigned char>& content);

		[[nodiscard]] const std::vector<unsigned char>& GetContent() const;

		[[nodiscard]] size_t GetSize() const;
};

class StringContent : public HttpContent {
	public:
		StringContent() = default;

		StringContent(const std::string& content); // NOLINT(google-explicit-constructor)

		StringContent(const std::string& content, const std::string& content_type); // NOLINT(google-explicit-constructor)

		~StringContent() = default;

		[[nodiscard]] std::tuple<std::string, size_t, const std::vector<unsigned char>> Serialize() const override;

		void SetContent(const std::string& content);

		void SetContentType(const std::string& content_type);

		[[nodiscard]] const std::string& GetContent() const;

		[[nodiscard]] const std::string& GetContentType() const;

		[[nodiscard]] size_t GetSize() const;

	private:
		std::string data_;
		std::string content_type_ = "text/plain";
};

class FormUrlEncodedContent : public HttpContent {
	public:
		FormUrlEncodedContent() = default;

		FormUrlEncodedContent( // NOLINT(google-explicit-constructor)
										const std::vector<std::pair<std::string, std::string>>& content);

		~FormUrlEncodedContent() = default;

		[[nodiscard]] static FormUrlEncodedContent Parse(const std::string& content);

		[[nodiscard]] std::tuple<std::string, size_t, const std::vector<unsigned char>> Serialize() const override;

		void SetContent(const std::vector<std::pair<std::string, std::string>>& content);

		void AddContent(const std::string& key, const std::string& value);

		[[nodiscard]] const std::vector<std::pair<std::string, std::string>>& GetContent() const;

		[[nodiscard]] size_t GetSize() const;

	private:
		std::vector<std::pair<std::string, std::string>> data_;
};

#ifdef CURLING_HAS_JSON

class JsonContent : public HttpContent {
	public:
		JsonContent() = default;

		JsonContent(const nlohmann::json& content); // NOLINT(google-explicit-constructor)

		template<typename T>
		JsonContent(const T& content) : data_(content) {} // NOLINT(google-explicit-constructor)

		~JsonContent() = default;

		[[nodiscard]] std::tuple<std::string, size_t, const std::vector<unsigned char>> Serialize() const override;

		void SetContent(const nlohmann::json& content);

		template<typename T>
		void SetContent(const T& content) {
				data_ = content;
		}

		[[nodiscard]] const nlohmann::json& GetContent() const;

		template<typename T>
		void GetContent(T& content) const {
				content = data_;
		}

	private:
		nlohmann::json data_;
};

#endif
}
}
