#include <sstream>
#include <iostream>
#include <algorithm>

#include <message_handle.hpp>

Server * server;

void create_server(const std::string& password) {
	server = new Server(password);
}

void delete_server(void) {
	delete server;
}

void delete_user(int client_fd) {
	server->remove_user(client_fd);
}

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
		if (argument == server->getPassword()) {
			server->add_user(client_fd);
			return (std::string(""));
		}
	}

	else if (command == "NICK") {
		token >> argument;
		if (!server->check_user(client_fd)) {
			return (":server 999 " + argument + " fuck you wrong password\n");
		} else {
			server->add_nick(client_fd, argument);
		}
	}

	else if (command == "USER") {
		token >> argument;
		if (!server->check_user(client_fd)) {
			return ("");
		} else {
			return (":server 999 " + server->get_nick(client_fd) + " Hello World\n");
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

bool Server::check_user(int client_fd) {
	return (std::find(fd_list.begin(), fd_list.end(), client_fd) != fd_list.end());
}

void Server::remove_user(int client_fd) {
	fd_list.remove(client_fd);
}

void Server::add_nick(int client_fd, const std::string& nick) {
	this->nick.insert(std::map<int, std::string>::value_type(client_fd, nick));
}

void Server::add_user(std::string nick, std::string user) {
	this->user.insert(std::map<std::string, std::string>::value_type(nick, user));
}

std::string Server::get_nick(int client_fd) {
	return nick.operator[](client_fd);
}

const std::string &Server::getPassword() const {
	return this->password;
}
