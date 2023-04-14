#pragma once

#include <memory>

#ifdef CURLING_HAS_TRACING

#include <opentelemetry/trace/tracer.h>

namespace tracing {
std::shared_ptr<opentelemetry::trace::Tracer> GetTracer();
}
#else
namespace opentelemetry::trace {
	enum class StatusCode {
		kOk,
		kError,
		kUnset
	};
}

class SpanMock {
public:
	static void SetStatus(opentelemetry::trace::StatusCode, const std::string&);
	static void SetAttribute(const std::string&, const std::string&);
};

class ScopeMock {
};

class TracerMock {
public:
	static std::shared_ptr<SpanMock> StartSpan(const std::string&);
	static ScopeMock WithActiveSpan(const std::shared_ptr<SpanMock>&);
};

namespace tracing {
std::shared_ptr<TracerMock> GetTracer();
}

#endif
