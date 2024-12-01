#include <Client.hpp>

Client::Client() {
	authenticated = false;
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
	return std::string("unknown command");
}

std::string Client::get_nick() {
	return this->nick;
}

std::string Client::get_user() {
	return this->user;
}

void Client::set_nick(const std::string &new_nick) {
	this->nick = new_nick;
}

void Client::set_user(const std::string &new_user) {
	this->nick = new_user;
}

void Client::set_hostname(const std::string &new_hostname) {
	this->hostname = new_hostname;
}

void Client::set_realname(const std::string &new_realname) {
	this->realname = new_realname;
}
