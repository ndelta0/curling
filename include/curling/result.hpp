#pragma once

#include <variant>
#include <optional>
#include <string_view>
#include <stdexcept>

#include "error.hpp"

namespace curling {
template<typename R, typename E>
class Result {
public:
	// Implicit conversion is desired here
	Result(R value) : value_(std::move(value)) {} // NOLINT(google-explicit-constructor)
	Result(const E& error) : value_(error) {} // NOLINT(google-explicit-constructor)

	[[nodiscard]] bool IsOk() const {
		return std::holds_alternative<R>(value_);
	}

	[[nodiscard]] bool IsError() const {
		return std::holds_alternative<E>(value_);
	}

	[[nodiscard]] std::optional<R> Ok() const {
		if (IsOk()) {
			return std::get<R>(value_);
		}
		return std::nullopt;
	}

	[[nodiscard]] std::optional<E> Error() const {
		if (IsError()) {
			return std::get<E>(value_);
		}
		return std::nullopt;
	}

	[[nodiscard]] R& Expect(const std::string_view& msg) {
		if (IsError()) {
			throw std::runtime_error(msg.data());
		}
		return std::get<R>(value_);
	}

	[[nodiscard]] R& Unwrap() {
		return Expect("Expected Ok, but got Error");
	}

	[[nodiscard]] R operator*() {
		return Unwrap();
	}

	[[nodiscard]] R* operator->() {
		return &Unwrap();
	}

	[[nodiscard]] explicit operator bool() const {
		return IsOk();
	}

	[[nodiscard]] bool operator!() const {
		return IsError();
	}

private:
	std::variant<R, E> value_;
};
}
