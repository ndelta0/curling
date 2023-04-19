#include <iostream>

#include <curling.hpp>
#include "curling/context.hpp"

using namespace curling;

int main()
{
	Context context;

	// Create a client
	auto client = std::move(context.CreateClient(Uri("https://httpbin.org/")).Expect("Failed to create client"));

	// Add a default header
	client->DefaultHeaders().SetHeader("Test-Header", "Test-Value");

	// Send a GET request
	auto response = client->Get(Uri("/get")).Expect("Failed to send request");

	// Print the response
	response.EnsureSuccessStatusCode();
	std::cout << "Status code: " << response.GetStatusCode() << "\n";

	// Print the response body
#if CURLING_HAS_JSON
	std::cout << "Body: " << response.GetContent().ReadAsJson().GetContent().dump(4) << std::endl;
#else
	std::cout << "Body: " << response.GetContent().ReadAsString().GetContent() << std::endl;
#endif

	return 0;
}
