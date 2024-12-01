#include <Server.hpp>

Server::Server(const std::string& password) {
	this->password = password;
}

Server::~Server() {

}

std::string Server::handle_message(const std::string& message, int client_fd) {
	std::string command;
	std::stringstream token(message);

	token >> command;

	std::cout << "[info]  | command received : " << command << std::endl;

	if (command == "PING")
		return std::string("PONG server");

	if (command == "CAP")
		return this->client.find(client_fd)->second.cap(token);

	return std::string("");
}

void Server::add_user(int client_fd) {
	this->client.insert(std::map<int, Client>::value_type(client_fd, Client()));
}

void Server::delete_user(int client_fd) {
	this->client.erase(client_fd);
}