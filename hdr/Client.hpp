#pragma once

#include <string>
#include <sstream>

class Client {
private:
	std::string nick;
	std::string user;
	std::string hostname;
	std::string realname;
public:
	Client();
	~Client();

	std::string cap(std::stringstream &token);

	std::string get_nick();
	std::string get_user();

	void set_nick(const std::string& new_nick);
	void set_user(const std::string& new_user);
	void set_hostname(const std::string& new_hostname);
	void set_realname(const std::string& new_realname);

	bool authenticated;
};
