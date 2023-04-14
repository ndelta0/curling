#pragma once

#include <string>
#include <vector>

class Utils {
public:
	[[nodiscard]] static std::string UrlEncode(const std::string& value);

	[[nodiscard]] static std::string UrlDecode(const std::string& value);

	[[nodiscard]] static std::string ToHex(unsigned char code);

	[[nodiscard]] static unsigned char FromHex(unsigned char code);

	[[nodiscard]] static std::vector<std::string> Split(const std::string& str, const std::string& delimiter);

	[[nodiscard]] static std::vector<std::string> Split(const std::string& str, char delimiter) {
		return Split(str, std::string(1, delimiter));
	};

	[[nodiscard]] static std::string Trim(const std::string& str, const std::string& whitespace = " \t");

	[[nodiscard]] static std::string LTrim(const std::string& str, const std::string& whitespace = " \t");

	[[nodiscard]] static std::string RTrim(const std::string& str, const std::string& whitespace = " \t");

	static std::string Join(std::vector<std::string> parts, char delimiter);

	static void RemoveEmpty(std::vector<std::string>& parts);
};
