#include <catch2/catch_test_macros.hpp>

#include <curling/context.hpp>

TEST_CASE("HttpFactory can be constructed", "[http_factory]") {
	curling::Context context;
	auto factory = context.CreateFactory();

	assert(factory.IsOk());
}

TEST_CASE("HttpFactory can create clients", "[http_factory]") {
	curling::Context context;
	auto factory_result = context.CreateFactory();
	if (!factory_result) {
		FAIL("Failed to create factory");
	}
	auto factory = *factory_result;

	auto client = factory->CreateClient();

	assert(client.IsOk());
}

TEST_CASE("Context can create clients", "[context]") {
	curling::Context context;

	auto client = context.CreateClient();

	assert(client.IsOk());
}
