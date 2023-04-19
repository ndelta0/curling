#pragma once

#include <mutex>

// Use std::stop_token when C++20 is available (__cpp_lib_jthread is defined)

namespace curling {
class CancellationToken {
public:
	CancellationToken();
	virtual ~CancellationToken();

	// Remove copy and move constructors
	CancellationToken(const CancellationToken&) = delete;
	CancellationToken& operator=(const CancellationToken&) = delete;
	CancellationToken(CancellationToken&&) = delete;
	CancellationToken& operator=(CancellationToken&&) = delete;

	explicit CancellationToken(bool cancelled) : cancelled_(cancelled) {}

	[[nodiscard]] bool IsCancelled();

	virtual void Cancel();

	void Reset();

	static CancellationToken& None();

private:
	bool cancelled_ = false;
	std::mutex mutex_;

	static CancellationToken *none_token_;
};

class NoneCancellationToken : public CancellationToken {
public:
	void Cancel() override {}

private:
	friend class CancellationToken;

	NoneCancellationToken() : CancellationToken(false) {}
};
}
