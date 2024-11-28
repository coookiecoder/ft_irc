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
	std::list<int> fd_list;
public:
	explicit Server(const std::string& password);
	~Server();

	std::string	handle_message(const std::string& message, int client_fd);

	void add_user(int client_fd);
	void delete_user(int client_fd);
	bool check_user(int client_fd);
	void remove_user(int client_fd);

	int add_nick(int client_fd, const std::string& nick);
	void add_user(int client_fd, std::string user);
	void add_hostname(int client_fd, std::string hostname);
	void add_realname(int client_fd, std::string realname);

	std::string get_nick(int client_fd);
	std::string get_user(int client_fd);

	const std::string &getPassword() const;
};
