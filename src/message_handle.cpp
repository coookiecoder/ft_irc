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


	else if (!server->check_user(client_fd)) {
		return (":server 999 " + argument + " fuck you wrong password\n");
	}

	else if (command == "NICK") {
		token >> argument;
		if (server->add_nick(client_fd, argument)) {
			server->remove_user(client_fd);
			return (":server 999 " + argument + " nick is already in use\n");
		}
	}

	else if (command == "USER") {
		token >> argument;
		server->add_user(client_fd, argument);
		return (":server 001 " + server->get_nick(client_fd) + " Hello World, registration in progress\n");
	}

	else if (command == "MODE") {
		token >> argument;
		if (argument != server->get_nick(client_fd)) {
			return (":server invalid MODE command\n");
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

Client::Client(std::string nick) {
	this->nick = nick;
}

std::string Client::get_nick(void) {
	return this->nick;
}

std::string Client::get_user(void) {
	return this->user;
}

void Client::set_nick(const std::string &new_nick) {
	this->nick = new_nick;
}

void Client::set_user(const std::string &new_user) {
	this->nick = new_user;
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
	this->client.erase(client_fd);
}

int Server::add_nick(int client_fd, const std::string& nick) {
	for (std::map<int, Client>::iterator iterator = this->client.begin(); iterator != this->client.end(); ++iterator) {
		if (iterator->second.get_nick() == nick)
			return (true);
	}
	this->client.insert(std::map<int, Client>::value_type(client_fd, Client(nick)));
	return (false);
}

void Server::add_user(int client_fd, std::string user) {
	this->client.find(client_fd)->second.set_user(user);
}

std::string Server::get_nick(int client_fd) {
	return this->client.find(client_fd)->second.get_nick();
}

std::string Server::get_user(int client_fd) {
	return this->client.find(client_fd)->second.get_user();
}

const std::string &Server::getPassword() const {
	return this->password;
}