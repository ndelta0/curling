#include <catch2/catch_test_macros.hpp>

#include <curling/uri.hpp>

TEST_CASE("Uri can be constructed", "[uri]") {
		curling::Uri uri("https://www.example"
																			".com:80/path/to/file?query=string&keytest&keytest2="
																		"#fragment");

		REQUIRE(uri.scheme == "https");
		REQUIRE(uri.username.empty());
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host == "www.example.com");
		REQUIRE(uri.port == 80);
		REQUIRE(uri.path == "/path/to/file");
		//REQUIRE(uri.query == "query=string");
		REQUIRE(uri.fragment == "fragment");
}

TEST_CASE("Uri can be constructed with user info", "[uri]") {
		curling::Uri uri("https://user:password@ftp.example.com:21/file.txt");

		REQUIRE(uri.scheme == "https");
		REQUIRE(uri.username == "user");
		REQUIRE(uri.password == "password");
		REQUIRE(uri.host == "ftp.example.com");
		REQUIRE(uri.port == 21);
		REQUIRE(uri.path == "/file.txt");
		REQUIRE(uri.query.empty());
		REQUIRE(uri.fragment.empty());
}

TEST_CASE("Uri can be constructed with empty user info", "[uri]") {
		curling::Uri uri("https://@ftp.example.com:21/file.txt");

		REQUIRE(uri.scheme == "https");
		REQUIRE(uri.username.empty());
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host == "ftp.example.com");
		REQUIRE(uri.port == 21);
		REQUIRE(uri.path == "/file.txt");
		REQUIRE(uri.query.empty());
		REQUIRE(uri.fragment.empty());
}

TEST_CASE("Uri can be constructed with empty password", "[uri]") {
		curling::Uri uri("https://user:@ftp.example.com:21/file.txt");

		REQUIRE(uri.scheme == "https");
		REQUIRE(uri.username == "user");
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host == "ftp.example.com");
		REQUIRE(uri.port == 21);
		REQUIRE(uri.path == "/file.txt");
		REQUIRE(uri.query.empty());
		REQUIRE(uri.fragment.empty());

		uri = curling::Uri("https://user@ftp.example.com:21/file.txt");

		REQUIRE(uri.scheme == "https");
		REQUIRE(uri.username == "user");
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host == "ftp.example.com");
		REQUIRE(uri.port == 21);
		REQUIRE(uri.path == "/file.txt");
		REQUIRE(uri.query.empty());
		REQUIRE(uri.fragment.empty());
}

TEST_CASE("Uri can be constructed with empty path", "[uri]") {
		curling::Uri uri("https://www.example.com:80?query=string#fragment");

		REQUIRE(uri.scheme == "https");
		REQUIRE(uri.username.empty());
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host == "www.example.com");
		REQUIRE(uri.port == 80);
		REQUIRE(uri.path.empty());
		//REQUIRE(uri.query == "query=string");
		REQUIRE(uri.fragment == "fragment");
}

TEST_CASE("Uri can be constructed with empty query", "[uri]") {
		curling::Uri uri("https://www.example.com:80/path/to/file#fragment");

		REQUIRE(uri.scheme == "https");
		REQUIRE(uri.username.empty());
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host == "www.example.com");
		REQUIRE(uri.port == 80);
		REQUIRE(uri.path == "/path/to/file");
		REQUIRE(uri.query.empty());
		REQUIRE(uri.fragment == "fragment");
}

TEST_CASE("Uri can be constructed with empty fragment", "[uri]") {
		curling::Uri uri("https://www.example.com:80/path/to/file?query=string");

		REQUIRE(uri.scheme == "https");
		REQUIRE(uri.username.empty());
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host == "www.example.com");
		REQUIRE(uri.port == 80);
		REQUIRE(uri.path == "/path/to/file");
		//REQUIRE(uri.query == "query=string");
		REQUIRE(uri.fragment.empty());
}

TEST_CASE("Uri can be constructed with empty port", "[uri]") {
		curling::Uri uri("https://www.example.com/path/to/file?query=string#fragment");

		REQUIRE(uri.scheme == "https");
		REQUIRE(uri.username.empty());
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host == "www.example.com");
		REQUIRE(uri.port == 0);
		REQUIRE(uri.path == "/path/to/file");
		//REQUIRE(uri.query == "query=string");
		REQUIRE(uri.fragment == "fragment");
}

TEST_CASE("Uri can be constructed with empty scheme", "[uri]") {
		curling::Uri uri("www.example.com:80/path/to/file?query=string#fragment");

		REQUIRE(uri.scheme.empty());
		REQUIRE(uri.username.empty());
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host == "www.example.com");
		REQUIRE(uri.port == 80);
		REQUIRE(uri.path == "/path/to/file");
		//REQUIRE(uri.query == "query=string");
		REQUIRE(uri.fragment == "fragment");
}

TEST_CASE("Uri can be relative", "[uri]") {
		curling::Uri uri("/path/to/file?query=string#fragment");

		REQUIRE(uri.scheme.empty());
		REQUIRE(uri.username.empty());
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host.empty());
		REQUIRE(uri.port == 0);
		REQUIRE(uri.path == "/path/to/file");
		//REQUIRE(uri.query == "query=string");
		REQUIRE(uri.fragment == "fragment");
}

TEST_CASE("Uri can be constructed with empty scheme and host", "[uri]") {
		curling::Uri uri("www.example.com");

		REQUIRE(uri.scheme.empty());
		REQUIRE(uri.username.empty());
		REQUIRE(uri.password.empty());
		REQUIRE(uri.host == "www.example.com");
		REQUIRE(uri.port	== 0);
		REQUIRE(uri.path.empty());
		REQUIRE(uri.query.empty());
		REQUIRE(uri.fragment.empty());
}
