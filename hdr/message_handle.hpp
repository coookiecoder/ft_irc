#pragma once

#include <string>
#include <map>
#include <list>

void create_server(const std::string& password);
void delete_server(void);
void delete_user(int client_fd);
std::string	handle_message(const std::string& message, int client_fd);

class Client {
private:
	std::string nick;
	std::string user;
public:
	Client(std::string nick);
	std::string get_nick(void);
	std::string get_user(void);

	void set_nick(const std::string& new_nick);
	void set_user(const std::string& new_user);
};

class Server {
private:
	std::string	password;
	std::map<int, Client> client;
	std::list<int> fd_list;
public:
	explicit Server(const std::string& password);
	~Server();

	void add_user(int client_fd);
	bool check_user(int client_fd);
	void remove_user(int client_fd);

	int add_nick(int client_fd, const std::string& nick);
	void add_user(int client_fd, std::string user);

	std::string get_nick(int client_fd);
	std::string get_user(int client_fd);

	const std::string &getPassword() const;
};
