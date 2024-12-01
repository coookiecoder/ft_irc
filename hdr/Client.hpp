#pragma once

#include <string>
#include <sstream>

class Client {
private:
	std::string nick;
	std::string user;
	std::string hostname;
	std::string realname;
	int fd;
public:
	Client(const std::string& client_fd, int fd);
	~Client();

	std::string cap(std::stringstream &token);

	std::string get_nick();
	std::string get_user();
	std::string get_host();

	std::string set_nick(const std::string& new_nick);
	std::string set_user(const std::string& new_user);
	std::string set_hostname(const std::string& new_hostname);
	std::string set_realname(const std::string& new_realname);

	bool operator==(const Client& opther);

	int get_fd();

	bool authenticated;
};
