#include <Client.hpp>

Client::Client(const std::string& nick) {
	this->nick = nick;
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
