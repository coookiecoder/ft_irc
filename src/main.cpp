#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>
#include <csignal>

int run = true;

void stop(int code) {
	if (code == SIGINT) {
		std::cout << std::endl << "[info]  | stopping server" << std::endl;
		run = false;
	}
}

int handle_client(int client_fd) {
	char buffer[1024];
	ssize_t response_size;

	response_size = recv(client_fd, buffer, sizeof(buffer), 0);
	if (response_size > 0)
		std::cout << buffer << std::endl;
	else {
		std::cout << "[info]  | connection closed" << std::endl;
		close(client_fd);
		return 1;
	}
	return 0;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "usage : ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	int port;
	std::stringstream(argv[1]) >> port;
	std::string password(argv[2]);

	if (std::strlen(argv[1]) > 5 || port > 65535 || port < 0) {
		std::cerr << "[error] | invalid port" << std::endl;
		return 1;
	}

	if (port <= 1024)
		std::cout << "[info]  | need root permission for port <= 1024, (will still try to use the port)" << std::endl;

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket == -1) {
		std::cerr << "[error] | unable to open server socket" << std::endl;
		return 1;
	}

	std::cout << "[info]  | server socket open" << std::endl;

	if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "[error] | unable to set the socket to non blocking";
		close(server_socket);
		return 1;
	}

	std::cout << "[info]  | server socket set to non blocking" << std::endl;

	struct sockaddr_in addr;

	std::memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(server_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) { 
		std::cerr << "[error] | unable to bind the socket" << std::endl;
		close(server_socket);
		return 1;
	}

	std::cout << "[info]  | sever bound" << std::endl;

	if (listen(server_socket, 32) == -1) {
		std::cerr << "[error] | unable to listen" << std::endl;
		close(server_socket);
		return 1;
	}

	std::cout << "[info]  | server listening" << std::endl;

	struct pollfd fds[1024];

	std::memset(fds, 0, sizeof(fds));

	fds[0].fd = server_socket;
	fds[0].events = POLLIN;

	signal(SIGINT, stop);

	int ready;

	while(run) {
		ready = poll(fds, 1024, -1);
		if (ready == -1 && run) {
			std::cerr << "[error] | unable to use poll" << std::endl;
			close(server_socket);
			return 1;
		}

		for (int i = 0; ready > 0; ++i) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == server_socket)
					fds[i].fd = accept(server_socket, NULL, NULL);
				if (handle_client(fds[i].fd))
					fds[i].fd = 0;
				ready--;
			}
		}
	}

	close(server_socket);

	return 0;
}
