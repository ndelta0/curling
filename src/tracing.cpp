#include "tracing.hpp"

#ifdef CURLING_HAS_TRACING

#include <opentelemetry/trace/provider.h>
#include "version.hpp"

std::shared_ptr<opentelemetry::trace::Tracer> tracing::GetTracer() {
	auto provider = opentelemetry::trace::Provider::GetTracerProvider();

	if (provider == nullptr) {
		return nullptr;
	}

	return provider->GetTracer("curling", CURLING_VERSION);
}

#else
std::shared_ptr<TracerMock> tracing::GetTracer() {
	return {};
}

void SpanMock::SetStatus(opentelemetry::trace::StatusCode, const std::string&) {}

void SpanMock::SetAttribute(const std::string&, const std::string&) {}

std::shared_ptr<SpanMock> TracerMock::StartSpan(const std::string&) {
	return {};
}

ScopeMock TracerMock::WithActiveSpan(const std::shared_ptr<SpanMock>&) {
	return {};
}
#endif
