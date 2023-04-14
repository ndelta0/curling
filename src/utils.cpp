#include "utils.hpp"

std::string Utils::UrlEncode(const std::string& value) {
	std::string encoded;
	encoded.reserve(value.size());
	for (char c: value) {
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			encoded += c;
		} else if (c == ' ') {
			encoded += '+';
		} else {
			encoded += '%';
			encoded += Utils::ToHex((c >> 4) & 0xf);
			encoded += Utils::ToHex(c & 0xf);
		}
	}
	return encoded;
}

std::string Utils::ToHex(unsigned char code) {
	static char hex[] = "0123456789ABCDEF";
	std::string encoded;
	encoded += hex[code & 15];
	return encoded;
}

std::string Utils::UrlDecode(const std::string& value) {
	std::string decoded;
	decoded.reserve(value.size());
	for (size_t i = 0; i < value.size(); ++i) {
		if (value[i] == '%') {
			if (i + 3 <= value.size()) {
				int code = 0;
#ifdef _WIN32
				if (sscanf_s(value.substr(i + 1, 2).c_str(), "%x", &code) != 1) {
						code = '?';
				}
#else
				if (strtoul(value.substr(i + 1, 2).c_str(), nullptr, 16) != (size_t) code) {
					code = '?';
				}
#endif
				decoded += static_cast<char>(code);
				i += 2;
			}
		} else if (value[i] == '+') {
			decoded += ' ';
		} else {
			decoded += value[i];
		}
	}
	return decoded;
}

unsigned char Utils::FromHex(unsigned char code) {
	return (unsigned char) (isdigit(code) ? code - '0' : tolower(code) - 'a' + 10);
}

std::vector<std::string> Utils::Split(const std::string& str, const std::string& delimiter) {
	if (str.empty()) {
		return {};
	}

	std::vector<std::string> result;
	auto start = 0ULL;
	size_t end;

	while ((end = str.find(delimiter, start)) != std::string::npos) {
		result.push_back(str.substr(start, end - start));
		start = end + 1;
	}

	result.push_back(str.substr(start));

	return result;
}

std::string Utils::LTrim(const std::string& str, const std::string& whitespace) {
	const auto str_begin = str.find_first_not_of(whitespace);
	if (str_begin == std::string::npos) {
		return "";
	}
	return str.substr(str_begin);
}

std::string Utils::RTrim(const std::string& str, const std::string& whitespace) {
	const auto str_end = str.find_last_not_of(whitespace);
	if (str_end == std::string::npos) {
		return "";
	}
	return str.substr(0, str_end + 1);
}

std::string Utils::Trim(const std::string& str, const std::string& whitespace) {
	return LTrim(RTrim(str, whitespace), whitespace);
}

std::string Utils::Join(std::vector<std::string> parts, char delimiter) {
	std::string result;
	for (size_t i = 0; i < parts.size(); i++) {
		result += parts[i];
		if (i < parts.size() - 1) {
			result += delimiter;
		}
	}
	return result;
}

void Utils::RemoveEmpty(std::vector<std::string>& parts) {
	for (size_t i = 0; i < parts.size(); i++) {
		if (parts[i].empty()) {
			parts.erase(parts.begin() + (long long) i);
			i--;
		}
	}
}
