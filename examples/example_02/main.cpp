#include <iostream>

#include <curling/context.hpp>

int main() {
		curling::Context context;
		auto client = context.CreateClient(curling::Uri("https://www.httpbin.org"));

		auto response = client->Get("/xml");

		std::cout << response.GetStatusCode() << std::endl;
		if (response) {
				std::cout << "OK (XML)" << std::endl;
				std::cout << response.GetHeaders().GetContentType() << std::endl;
				std::cout << response.GetHeaders().GetContentLength() << std::endl;
				std::cout << response.GetContent().ReadAsString().GetContent() << std::endl;
		} else {
				std::cout << "Error" << std::endl;
		}

		response = client->Get("/json");

		std::cout << response.GetStatusCode() << std::endl;
		if (response) {
				std::cout << "OK (JSON)" << std::endl;
				std::cout << response.GetHeaders().GetContentType() << std::endl;
				std::cout << response.GetHeaders().GetContentLength() << std::endl;
				std::cout << response.GetContent().ReadAsString().GetContent() << std::endl;
		} else {
				std::cout << "Error" << std::endl;
		}

		return 0;
}
