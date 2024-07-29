#pragma once

#include <string>
#include <map>
#include <list>

void create_server(const std::string& password);
void delete_server(void);
void delete_user(int client_fd);
std::string	handle_message(const std::string& message, int client_fd);

class Server {
private:
	std::string	password;
	std::map<std::string, std::string> user;
	std::map<int, std::string> nick;
	std::list<int> fd_list;
public:
	explicit Server(const std::string& password);
	~Server();

	void add_user(int client_fd);
	bool check_user(int client_fd);
	void remove_user(int client_fd);

	int add_nick(int client_fd, const std::string& nick);
	void add_user(std::string nick, std::string user);

	std::string get_nick(int client_fd);

	const std::string &getPassword() const;
};
