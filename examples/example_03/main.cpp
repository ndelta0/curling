#include <iostream>

#include <curling/context.hpp>

int main() {
	curling::Context context;
	auto client = std::move(
			context.CreateClient(curling::Uri("https://www.httpbin.org/status/")).Expect("Cannot create client"));

	auto response = client->Get("/200").Expect("Cannot GET /200");

	std::cout << response.GetStatusCode() << std::endl;
	if (response) {
		std::cout << "OK (200)" << std::endl;
		std::cout << response.GetHeaders().GetContentType() << std::endl;
		std::cout << response.GetHeaders().GetContentLength() << std::endl;
		std::cout << response.GetContent().ReadAsString().GetContent() << std::endl;
	} else {
		std::cout << "Error" << std::endl;
	}

	response = client->Get("/201").Expect("Cannot GET /201");

	std::cout << response.GetStatusCode() << std::endl;
	if (response) {
		std::cout << "OK (201)" << std::endl;
		std::cout << response.GetHeaders().GetContentType() << std::endl;
		std::cout << response.GetHeaders().GetContentLength() << std::endl;
		std::cout << response.GetContent().ReadAsString().GetContent() << std::endl;
	} else {
		std::cout << "Error" << std::endl;
	}

	return 0;
}
