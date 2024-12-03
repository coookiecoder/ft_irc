#include <Client.hpp>

Client::Client(const std::string& client_fd, int fd) {
	authenticated = false;
	nick = client_fd;
	user = "";
	hostname = "";
	realname = "";
	this->fd = fd;
}

Client::~Client() {

}

std::string Client::cap(std::stringstream &token) {
	std::string buffer;

	token >> buffer;

	if (buffer == "LS")
		return std::string("CAP * LS :none\n");
	if (buffer == "END" && authenticated)
		return std::string(":server 001 " + this->nick + " Welcome to the sever\n");
	return std::string("");
}

std::string Client::get_nick() {
	return this->nick;
}

std::string Client::get_user() {
	return this->user;
}

std::string Client::get_host() {
	return this->hostname;
}

std::string Client::set_nick(const std::string &new_nick) {
	std::string buffer(":" + this->get_nick() + "!" + this->get_user() + "@" + this->get_host() + " NICK :" + new_nick + "\n");
	this->nick = new_nick;
	return std::string(buffer);
}

std::string Client::set_user(const std::string &new_user) {
	this->nick = new_user;
	return std::string("");
}

std::string Client::set_hostname(const std::string &new_hostname) {
	this->hostname = new_hostname;
	return std::string("");
}

std::string Client::set_realname(const std::string &new_realname) {
	this->realname = new_realname;
	return std::string("");
}

bool Client::operator==(const Client& other_client) {
	return this->nick == other_client.nick;
}

int Client::get_fd() {
	return fd;
}
