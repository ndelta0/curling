#pragma once

#include <string>
#include <map>
#include <optional>

namespace curling {
class HttpHeaders {
public:
	HttpHeaders() = default;

	~HttpHeaders() = default;

	void SetHeader(const std::string& key, const std::string& value);

	void RemoveHeader(const std::string& key);

	void ClearHeaders();

	[[nodiscard]] const std::map<std::string, std::string>& GetHeaders() const;

	[[nodiscard]] std::optional<const std::string> GetHeader(const std::string& key) const;

	[[nodiscard]] bool HasHeader(const std::string& key) const;

protected:
	[[nodiscard]] static std::string NormalizeKey(const std::string& key);

	std::map<std::string, std::string> headers_;
};
}
