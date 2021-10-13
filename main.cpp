#include "Client.h"

int main()
{
	try {
		Client* client = new Client();
		client->main();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		std::cout << TER_MSG << std::endl;
		return 1;
	}
	return 0;
}
