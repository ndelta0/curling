#include <iostream>

#include <curling.hpp>
#include <curling/context.hpp>
#include "curling/http_content.hpp"

int main() {
	curling::Context context;
	auto client = std::move(context.CreateClient(curling::Uri("https://www.httpbin.org/")).Expect("Cannot create client"));

	curling::HttpRequestMessage request(curling::HttpMethod::kPost, "/post?foo=bar&baz");
	request.content = std::make_shared<curling::content::StringContent>("Hello World!");
	request.headers.SetUserAgent("SampleUserAgent/1.0");

	auto response = client->Send(request).Expect("Cannot send request");

	std::cout << response.GetStatusCode() << std::endl;
	if (response) {
		std::cout << "OK" << std::endl;
		std::cout << response.GetHeaders().GetContentType() << std::endl;
		std::cout << response.GetHeaders().GetContentLength() << std::endl;
		std::cout << response.GetContent().ReadAsString().GetContent() << std::endl;
	} else {
		std::cout << "Error" << std::endl;
	}

#ifdef CURLING_HAS_JSON
	request = curling::HttpRequestMessage(curling::HttpMethod::kPatch, "/patch");
	request.content = std::make_shared<curling::content::JsonContent>(nlohmann::json({{"foo", "bar"},
	                                                                                  {"baz", 42}}));

	response = client->Send(request).Expect("Cannot send request");

	std::cout << response.GetStatusCode() << std::endl;
	if (response) {
		std::cout << "OK" << std::endl;
		std::cout << response.GetHeaders().GetContentType() << std::endl;
		std::cout << response.GetHeaders().GetContentLength() << std::endl;
		std::cout << response.GetContent().ReadAsJson().GetContent().dump(1) << std::endl;
	} else {
		std::cout << "Error" << std::endl;
	}
#endif

	return 0;
}
