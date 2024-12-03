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

	std::cout << "[info]  | command received from " << this->client.find(client_fd)->second.get_nick() << " : " << command << std::endl;

	if (command == "PING")
		return std::string("PONG server\n");

	if (command == "PASS") {
		token >> command;
		this->client.find(client_fd)->second.authenticated = command == this->password;
		return std::string("");
	}

	if (command == "CAP")
		return this->client.find(client_fd)->second.cap(token);

	if (this->client.find(client_fd)->second.authenticated == false)
		return std::string (":server 451 * :You have not registered\n");

	if (command == "NICK") {
		token >> command;
		if (!command.empty() && command != "NICK") {
			if (command == this->client.find(client_fd)->second.get_nick())
				return std::string("");
			for (std::map<int, Client>::iterator iterator = this->client.begin(); iterator != this->client.end(); iterator++) {
				if (iterator->second.get_nick() == command)
					return std::string(":server 433 * " + command + " :Nickname is already in use\n");
			}
			return this->client.find(client_fd)->second.set_nick(command);
		}
		return std::string("");
	}

	if (command == "USER") {
		token >> command;
		if (!command.empty() && command != "USER") {
			return this->client.find(client_fd)->second.set_user(command);
		}
		return std::string("");
	}

	if (command == "JOIN") {
		token >> command;
		if (!command.empty() && command != "JOIN") {
			if (command[0] != '#')
				command = "#" + command;
			// Check if the channel already exists
			for (std::list<Channel>::iterator iterator = this->channel.begin(); iterator != this->channel.end(); iterator++) {
				if (iterator->get_name() == command) {
					std::string password;
					token >> password;
					iterator->add_member(this->client.find(client_fd)->second, password);
					std::cout << "[info]  | " << this->client.find(client_fd)->second.get_nick() << " joined " << command << std::endl;
					return std::string("");
				}
			}
			// Create a new channel
			this->channel.push_back(Channel(command, this->client.find(client_fd)->second));
			std::string password;
			token >> password;
			this->channel.back().add_member(this->client.find(client_fd)->second, password);
			std::cout << "[info]  | " << this->client.find(client_fd)->second.get_nick() << " created " << command << std::endl;
			return std::string("");
		}
	}

	if (command == "PRIVMSG") {
		token >> command;
		size_t pos = message.find(':');
		if (!command.empty() && pos != std::string::npos && command != "PRIVMSG" && command[0] == '#') {
			std::cout << "[info]  | message for channel " << command << std::endl;
			//send to a channel
			std::string message_to_client = message.substr(pos + 1);
			for (std::list<Channel>::iterator iterator = this->channel.begin(); iterator != this->channel.end(); iterator++) {
				if (iterator->get_name() == command && iterator->is_member(client_fd)) {
					for (std::list<Client>::iterator client_list = iterator->get_member_begin(); client_list != iterator->get_member_end(); client_list++) {
						if (client_list->get_fd() == client_fd)
							continue;
						std::string buffer(":" + this->client.find(client_fd)->second.get_nick() + "!" + this->client.find(client_fd)->second.get_user() + "@" + this->client.find(client_fd)->second.get_host() + " PRIVMSG " + command + " :" + message_to_client + "\n");
						send(client_list->get_fd(), buffer.c_str(), buffer.length(), MSG_DONTWAIT);
						std::cout << "[info]  | " << this->client.find(client_fd)->second.get_nick() << " sent a message to " << client_list->get_fd() << std::endl;
					}
					return std::string("");
				} else if (iterator->get_name() == command && !iterator->is_member(client_fd)) {
					std::cout << "[info]  | " << this->client.find(client_fd)->second.get_nick() << " is not in the channel " << iterator->get_name() << std::endl;
				}
			}
		}
		else if (!command.empty() && pos != std::string::npos && command != "PRIVMSG" && command[0] != '#') {
			std::cout << "[info]  | message for user" << command << std::endl;
			//send to a user
			std::string message_to_client = message.substr(pos + 1);
			for (std::map<int, Client>::iterator client_list = this->client.begin(); client_list != this->client.end(); client_list++) {
				if (client_list->second.get_nick() == command) {
					std::string buffer(":" + this->client.find(client_fd)->second.get_nick() + "!" + this->client.find(client_fd)->second.get_user() + "@" + this->client.find(client_fd)->second.get_host() + " PRIVMSG " + command + " :" + message_to_client + "\n");
					send(client_list->first, buffer.c_str(), buffer.length(), MSG_DONTWAIT);
					std::cout << "[info]  | " << this->client.find(client_fd)->second.get_nick() << " sent a message to " << client_list->first << std::endl;
				}
			}
			return std::string("");
		}
	}

	if (command == "KICK") {
		token >> command;
		if (!command.empty() && command != "KICK") {
			for (std::list<Channel>::iterator iterator = this->channel.begin(); iterator != this->channel.end(); iterator++) {
				if (iterator->get_name() == command && iterator->is_member(client_fd)) {
					token >> command;
					iterator->kick(command, this->client.find(client_fd)->second, message);
					return std::string("");
				} else if (iterator->get_name() == command && !iterator->is_member(client_fd)) {
					std::cout << "[info]  | " << this->client.find(client_fd)->second.get_nick() << " is not in the channel " << iterator->get_name() << std::endl;
				}
			}
		}
	}

	if (command == "TOPIC") {
		token >> command;
		if (!command.empty() && command != "TOPIC") {
			for (std::list<Channel>::iterator iterator = this->channel.begin(); iterator != this->channel.end(); iterator++) {
				if (iterator->get_name() == command && iterator->is_member(client_fd)) {
					return (iterator->set_topic(message, this->client.find(client_fd)->second));
				} else if (iterator->get_name() == command && !iterator->is_member(client_fd)) {
					std::cout << "[info]  | " << this->client.find(client_fd)->second.get_nick() << " is not in the channel " << iterator->get_name() << std::endl;
				}
			}	
		}
	}

	return std::string("");
}

void Server::add_user(int client_fd) {
	std::stringstream ss;
	ss << client_fd;
	this->client.insert(std::map<int, Client>::value_type(client_fd, Client(ss.str(), client_fd)));
}

void Server::delete_user(int client_fd) {
	this->client.erase(client_fd);
	for (std::list<Channel>::iterator iterator = this->channel.begin(); iterator != this->channel.end(); iterator++) {
		iterator->del_user(this->client.find(client_fd)->second);
	}	
}
