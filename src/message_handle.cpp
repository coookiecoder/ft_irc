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

	if (command == "PING") {
		token >> argument;
		if (argument == "server") {
			return ("PONG server\n");
		}
	}

	else if (command == "CAP") {
		token >> argument;
		if (argument == "LS") {
			return (std::string("CAP * LS :none\n"));
		} else if (argument == "END") {
			return (std::string(":server 001 " + server->get_nick(client_fd) + " Welcome to the sever\n"));
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
			if (server->add_nick(client_fd, argument)) {
				server->remove_user(client_fd);
				return (":server 999 " + argument + " nick is already in use\n");
			}
		}
	}

	else if (command == "USER") {
		token >> argument;
		if (!server->check_user(client_fd)) {
			return ("");
		} else {
			server->add_user(server->get_nick(client_fd), argument);
			return (":server 001 " + server->get_nick(client_fd) + " Hello World, registration in progress\n");
		}
	}

	else if (command == "MODE") {
		token >> argument;
		if (!server->check_user(client_fd) || argument != server->get_nick(client_fd)) {
			return ("");
		} else {
			token >> argument;
		}

		if (argument == "+i") {
			return (":server MODE " + server->get_nick(client_fd) + " +i\n");
		} else if (argument == "-i") {
			return (":server MODE " + server->get_nick(client_fd) + " -i\n");
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
	this->user.erase(this->get_nick(client_fd));
	this->nick.erase(client_fd);
}

int Server::add_nick(int client_fd, const std::string& nick) {
	for (std::map<int, std::string>::iterator iterator = this->nick.begin(); iterator != this->nick.end(); ++iterator) {
		if (iterator->second == nick)
			return (true);
	}
	this->nick.insert(std::map<int, std::string>::value_type(client_fd, nick));
	return (false);
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
