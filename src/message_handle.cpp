#include <sstream>
#include <iostream>
#include <algorithm>

#include <message_handle.hpp>

std::string	handle_message(const std::string& message, int client_fd) {
	std::string command;
	std::string argument;
	std::stringstream token(message);

	token >> command;

	std::cout << "[info]  | command received : " << command << std::endl;

	if (command == "CAP") {
		token >> argument;
		if (argument == "LS") {
			return (std::string("CAP * LS :none\n"));
		}
	}

	else if (command == "PASS") {
		token >> argument;
		if (argument == server.getPassword()) {
			server.add_user(client_fd);
			return (std::string(""));
		}
	}

	else if (command == "NICK") {
		token >> argument;
		if (!server.check_user(client_fd)) {
			return (":server 999 " + argument + " fuck you wrong password");
		} else {

		}
	}

	return (std::string(""));
}

Server::Server(const std::string& password) {
	this->password = password;
}

Server::~Server() {

}

void Server::add_user(int client_fd) {
	fd_list.push_front(client_fd);
}

void Server::remove_user(int client_fd) {
	fd_list.remove(client_fd);
}

bool Server::check_user(int client_fd) {
	return (std::find(fd_list.begin(), fd_list.end(), client_fd) != fd_list.end());
}

const std::string &Server::getPassword() const {
	return this->password;
}
