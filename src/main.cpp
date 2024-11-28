#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>
#include <csignal>

#include <Server.hpp>

#define nullptr NULL

int run = true;

void stop(int code) {
	if (code == SIGINT) {
		std::cout << std::endl << "[info]  | stopping server" << std::endl;
		run = false;
	}
}

int handle_client(Server &server, int client_fd) {
	char buffer[513];
	ssize_t response_size;

	response_size = recv(client_fd, buffer, sizeof(buffer), 0);
	if (response_size > 0) {
		std::cout << "[info]  | message received : " << std::endl;
		std::cout << buffer << std::endl;
		std::cout << "[info]  | end of message" << std::endl;
		std::stringstream message(buffer);
		std::string line;
		std::string response;
		while(std::getline(message,line,'\n')){
			response = server.handle_message(line, client_fd);
			if (!response.empty()) {
				std::cout << "[info]  | message sent : " << std::endl;
				std::cout << response << std::endl;
				send(client_fd, response.c_str(), response.length(), MSG_DONTWAIT);
				std::cout << "[info]  | end of message" << std::endl;
			}
		}
	}
	else {
		std::cout << "[info]  | connection closed fd : " << client_fd << std::endl;
		server.delete_user(client_fd);
		close(client_fd);
		return 1;
	}

	std::memset(buffer, 0, 512);

	return 0;
}

void remove_closed(pollfd *fds, int* nfds, int client_index) {
	std::cout << "[info]  | removing client index : " << client_index << std::endl;
	for (int i = client_index; i < *nfds - 1; ++i) {
		fds[i] = fds[i + 1];
	}
	(*nfds)--;
	fds[*nfds].fd = -1;
	fds[*nfds].events = 0;
	fds[*nfds].revents = 0;
}


int main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "usage : ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	int port;
	std::stringstream(argv[1]) >> port;

	Server server(argv[2]);

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

	int socket_option = 1;

	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_option, sizeof(socket_option))) {
		std::cerr << "[error] | unable to make the socket reusable";
		close(server_socket);
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
	int nfds = 1;

	while(run) {
		ready = poll(fds, nfds, -1);
		if (ready == -1 && run) {
			std::cerr << "[error] | unable to use poll" << std::endl;
			close(server_socket);
			return 1;
		}

		for (int i = 0; i < nfds && ready > 0; ++i) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == server_socket) {
					int new_client = accept(server_socket, nullptr, nullptr);
					if (new_client == -1) {
						std::cerr << "[error] | unable to accept new client" << std::endl;
					} else {
						std::cout << "[info]  | new client connected fd : " << new_client << std::endl;
						fcntl(new_client, F_SETFL, O_NONBLOCK);
						fds[nfds].fd = new_client;
						fds[nfds].events = POLLIN;
						nfds++;
					}
				} else {
					if (handle_client(server, fds[i].fd)) {
						remove_closed(fds, &nfds, i);
						i--; // Stay on the same index to check the next item after removal
					}
				}
				ready--;
			}
		}
	}

	while (nfds > 1) {
		std::cout << "[info]  | client disconnected fd : " << fds[1].fd << std::endl;
		close(fds[1].fd);
		remove_closed(fds, &nfds, 1);
	}
	close(server_socket);

	return 0;
}
