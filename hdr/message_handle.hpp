#pragma once

#include <string>
#include <list>

std::string	handle_message(const std::string& message, int client_fd);

class Server {
private:
	std::string	password;
	std::list<std::string> user;
	std::list<std::string> nick;
	std::list<int> fd_list;
public:
	explicit Server(const std::string& password);
	~Server();

	void add_user(int client_fd);
	bool check_user(int client_fd);

	const std::string &getPassword() const;
};

Server * server;
