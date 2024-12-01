#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <list>
#include <algorithm>

#include <Client.hpp>

class Server {
private:
	std::string	password;
	std::map<int, Client> client;
public:
	explicit Server(const std::string& password);
	~Server();

	std::string	handle_message(const std::string& message, int client_fd);

	void add_user(int client_fd);
	void delete_user(int client_fd);
};
