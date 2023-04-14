#include "uri.hpp"
#include "utils.hpp"

#include <string>
#include <regex>

curling::Uri::Uri(const std::string& uri) {
	std::smatch matches;

	if (std::regex_match(uri, matches, uri_regex)) {
		scheme = matches[1].str();
		username = matches[2].str();
		password = matches[3].str();
		host = matches[4].str();
		if (!host.empty() && host[0] == '@') {
			host = host.substr(1);
		}

		if (!matches[5].str().empty()) {
			port = static_cast<uint16_t>(std::stoi(matches[5].str()));
		}
		path = matches[6].str();

		auto query_string = matches[7].str();
		auto query_pairs = Utils::Split(query_string, '&');
		for (const auto& pair: query_pairs) {
			auto key_value = Utils::Split(pair, '=');

			if (key_value.size() == 1) {
				query.emplace_back(key_value[0], "");
			} else if (key_value.size() == 2) {
				if (key_value[0].empty()) {
					throw std::invalid_argument("Invalid query");
				} else {
					query.emplace_back(key_value[0], key_value[1]);
				}
			} else {
				throw std::invalid_argument("Invalid query");
			}
		}

		fragment = matches[8].str();
	} else {
		throw std::invalid_argument("Invalid URI");
	}
}

std::string curling::Uri::ToString() const {
	std::string result;

	if (!scheme.empty()) {
		result += scheme + "://";
	}

	if (!username.empty()) {
		result += Utils::UrlEncode(username);

		if (!password.empty()) {
			result += ":" + Utils::UrlEncode(password);
		}

		result += "@";
	}

	// TODO: Punycode
	result += host;

	if (port != 0) {
		result += ":" + std::to_string(port);
	}

	if (!path.empty()) {
		auto parts = Utils::Split(path, '/');
		Utils::RemoveEmpty(parts);
		for (const auto& part: parts) {
			result += "/" + Utils::UrlEncode(part);
		}

		// If last 2 characters are '/', remove one
		if (result.size() >= 2 && result[result.size() - 1] == '/' &&
		    result[result.size() - 2] == '/') {
			result.pop_back();
		}
	}

	if (!query.empty()) {
		result += "?";

		for (auto it = query.begin(); it != query.end(); ++it) {
			result += Utils::UrlEncode(it->first);
			if (!it->second.empty()) {
				result += "=" + Utils::UrlEncode(it->second);
			}

			if (std::next(it) != query.end()) {
				result += "&";
			}
		}
	}

	if (!fragment.empty()) {
		result += "#" + Utils::UrlEncode(fragment);
	}

	return result;
}

bool curling::Uri::IsRelative() const {
	return scheme.empty() && username.empty() && password.empty() &&
	       host.empty() && port == 0 && !path.empty();
}

bool curling::Uri::IsAbsolute() const {
	return !scheme.empty() && !host.empty() && (!username.empty() ||
	                                            (username.empty() && !password.empty()));
}

curling::Uri curling::Uri::operator+(const curling::Uri& other) const {
	if (IsRelative()) {
		throw std::invalid_argument("Cannot concatenate URI to relative URI");
	}
	if (IsAbsolute() && !IsAbsoluteNoQueryOrFragment()) {
		throw std::invalid_argument("Cannot concatenate URI to absolute URI with query or fragment");
	}
	if (other.IsAbsolute()) {
		throw std::invalid_argument("Cannot concatenate absolute URI to URI");
	}

	Uri result;
	result.scheme = scheme;
	result.username = username;
	result.password = password;
	result.host = host;
	result.port = port;

	if (!path.empty() && path.back() == '/') {
		// If path ends with '/', remove it
		result.path = path.substr(0, path.size() - 1);
		result.path += other.path;
	} else {
		result.path = path + other.path;
	}

	result.query = other.query;
	result.fragment = other.fragment;
	return result;
}

bool curling::Uri::IsAbsoluteNoQueryOrFragment() const {
	return IsAbsolute() && query.empty() && fragment.empty();
}

std::string curling::Uri::GetAuthority() const {
	std::string result = host;

	if (port != 0) {
		result += ":" + std::to_string(port);
	}

	return result;
}
