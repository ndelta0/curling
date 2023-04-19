#include <iostream>

#include <curling/context.hpp>

int main() {
	curling::Context context;
	auto factory = context.CreateFactory().Expect("Cannot create factory");

	auto client = std::move(factory->CreateClient().Expect("Cannot create client"));

	auto response = client->Get("https://www.example.com/").Expect("Cannot GET https://www.example.com/");

	std::cout << response.GetStatusCode() << std::endl;
	if (response) {
		std::cout << "OK" << std::endl;
		std::cout << response.GetHeaders().GetContentType() << std::endl;
		std::cout << response.GetHeaders().GetContentLength() << std::endl;
		std::cout << response.GetContent().ReadAsString().GetContent() << std::endl;
	} else {
		std::cout << "Error" << std::endl;
	}

	return 0;
}
