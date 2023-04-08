#include <catch2/catch_test_macros.hpp>

#include <curling.hpp>
#include <curling/context.hpp>

TEST_CASE("HttpClient can GET https://www.example.com/", "[http_client][get]") {
		curling::Context context;

		auto factory	= context.CreateFactory();

		auto client = factory->CreateClient();
		auto result = client->GetString(curling::Uri("https://www.example.com/"));

		assert(result.has_value());
		assert(result.value().find("<!doctype html>") != std::string::npos);
}
