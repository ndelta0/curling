#pragma once

#include <string>
#include <regex>

namespace curling {
struct Uri {
		Uri() = default;
		Uri(const std::string& uri); // NOLINT(google-explicit-constructor)

		~Uri() = default;

		[[nodiscard]] std::string ToString() const;
		[[nodiscard]] bool IsRelative() const;
		[[nodiscard]] bool IsAbsolute()	const;
		[[nodiscard]] bool IsAbsoluteNoQueryOrFragment() const;

		std::string scheme;
		std::string username;
		std::string password;
		std::string host;
		uint16_t port = 0;
		std::string path;
		std::vector<std::pair<std::string, std::string>> query;
		std::string fragment;

		Uri operator+(const Uri& other) const;

	private:
		std::regex uri_regex = std::regex(
			R"(^(?:([a-z]+):\/\/)?(?:([^:@\s]+)(?::([^:@\s]+)?)?@)?(?:([^\s:?#\/]+)(?::(\d{1,5}))?)?(\/[^\s?#]*)?(?:\?([^\s#]+))?(?:#([^\s]+))?$)",
   std::regex_constants::optimize | std::regex_constants::ECMAScript);
};
}
