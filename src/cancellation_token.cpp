#include "cancellation_token.hpp"

bool curling::CancellationToken::IsCancelled() {
	std::lock_guard lock(mutex_);
	return cancelled_;
}

void curling::CancellationToken::Cancel() {
	std::lock_guard lock(mutex_);
	cancelled_ = true;
}

void curling::CancellationToken::Reset() {
	std::lock_guard lock(mutex_);
	cancelled_ = false;
}

curling::CancellationToken::CancellationToken() {
	none_token_ = new NoneCancellationToken();
}

curling::CancellationToken::~CancellationToken() {
	delete none_token_;
}

curling::CancellationToken& curling::CancellationToken::None() {
	return *none_token_;
}
