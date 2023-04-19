#include <iostream>

#include <curling/context.hpp>

int main() {
	curling::Context context;
	auto client = std::move(context.CreateClient(curling::Uri("https://www.httpbin.org")).Expect("Cannot create client"));

	auto response = client->Get("/xml").Expect("Cannot get /xml");

	std::cout << response.GetStatusCode() << std::endl;
	if (response) {
		std::cout << "OK (XML)" << std::endl;
		std::cout << response.GetHeaders().GetContentType() << std::endl;
		std::cout << response.GetHeaders().GetContentLength() << std::endl;
		std::cout << response.GetContent().ReadAsString().GetContent() << std::endl;
	} else {
		std::cout << "Error" << std::endl;
	}

#ifdef CURLING_HAS_JSON
	response = client->Get("/json").Expect("Cannot get /json");

	std::cout << response.GetStatusCode() << std::endl;
	if (response) {
		std::cout << "OK (JSON)" << std::endl;
		std::cout << response.GetHeaders().GetContentType() << std::endl;
		std::cout << response.GetHeaders().GetContentLength() << std::endl;
		std::cout << response.GetContent().ReadAsString().GetContent() << std::endl;
	} else {
		std::cout << "Error" << std::endl;
	}
#endif

	return 0;
}
