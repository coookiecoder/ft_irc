#include <Server.hpp>

std::string Server::handle_message(const std::string& message, int client_fd) {
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
		} else if (argument == "END" && this->check_user(client_fd)) {
			return (std::string(":server 001 " + this->get_nick(client_fd) + " Welcome to the sever\n"));
		}
	}

	else if (command == "PASS") {
		token >> argument;
		if (argument == this->getPassword()) {
			this->add_user(client_fd);
			return (std::string(""));
		}
	}

	else if (!this->check_user(client_fd)) {
		return (":server 999 " + argument + " invalid command or wrong password received\n");
	}

	else if (command == "NICK") {
		token >> argument;
		if (this->add_nick(client_fd, argument)) {
			this->remove_user(client_fd);
			return (":server 999 " + argument + " nick is already in use or you already sent your nick\n");
		}
	}

	else if (command == "USER") {
		token >> argument;
		this->add_user(client_fd, argument);
		token >> argument;
		this->add_hostname(client_fd, argument);
		token >> argument;
		std::getline(token, argument);
		this->add_realname(client_fd, argument);
		return (":server 001 " + this->get_nick(client_fd) + " Hello World, registration in progress\n");
	}

	else if (command == "MODE") {
		token >> argument;
		if (argument != this->get_nick(client_fd)) {
			return (":server invalid MODE command\n");
		} else {
			token >> argument;
		}

		if (argument == "+i") {
			return (":server MODE " + this->get_nick(client_fd) + " +i\n");
		} else if (argument == "-i") {
			return (":server MODE " + this->get_nick(client_fd) + " -i\n");
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

void Server::delete_user(int client_fd) {
	fd_list.remove(client_fd);
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
	if (this->client.find(client_fd) != this->client.end())
		return (true);
	this->client.insert(std::map<int, Client>::value_type(client_fd, Client(nick)));
	return (false);
}

void Server::add_user(int client_fd, std::string user) {
	this->client.find(client_fd)->second.set_user(user);
}

void Server::add_hostname(int client_fd, std::string hostname) {
	this->client.find(client_fd)->second.set_hostname(hostname);
}

void Server::add_realname(int client_fd, std::string realname) {
	realname = realname.c_str() + 2;
	this->client.find(client_fd)->second.set_realname(realname);
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