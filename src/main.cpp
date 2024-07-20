#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sstream>

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "usage : ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	int port;
	std::stringstream(argv[1]) >> port;
	std::string password(argv[2]);

	if (std::strlen(argv[1]) > 5 || port > 65535 || port < 0) {
		std::cout << "invalid port : " << port << std::endl;
		return 1;
	}

	if (port <= 1024)
		std::cout << "need root permission for port <= 1024, (will still try to use the port)" << std::endl;

	return 0;
}
