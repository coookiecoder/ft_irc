#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <list>
#include <algorithm>

#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>
#include <csignal>


#include <Client.hpp>
#include <Channel.hpp>

class Server {
private:
	std::string	password;
	std::map<int, Client> client;
	std::list<Channel> channel;
public:
	explicit Server(const std::string& password);
	~Server();

	std::string	handle_message(const std::string& message, int client_fd);

	void add_user(int client_fd);
	void delete_user(int client_fd);
};
