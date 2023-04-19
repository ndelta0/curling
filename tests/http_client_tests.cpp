#include <catch2/catch_test_macros.hpp>

#include <curling.hpp>
#include <curling/context.hpp>

TEST_CASE("HttpClient can GET https://www.example.com/", "[http_client][get]") {
	curling::Context context;

	auto factory_result = context.CreateFactory();
	if (!factory_result) {
		FAIL("Failed to create factory");
	}
	auto factory = *factory_result;

	auto client_result = factory->CreateClient();
	if (!client_result) {
		FAIL("Failed to create client");
	}
	auto client = std::move(client_result.Unwrap());

	auto result = client->GetString(curling::Uri("https://www.example.com/"));

	assert(result);
	assert(result.Unwrap().find("<!doctype html>") != std::string::npos);
}
