#include <Channel.hpp>

Channel::Channel(const std::string &name, Client& user) {
    this->name = name;
	this->operator_member.push_back(user);
}

Channel::~Channel() {

}

void Channel::set_name(const std::string &name) {
    this->name = name;
}

void Channel::set_topic(const std::string &topic, Client& user) {
    if (topic_restriction && find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        this->topic = topic;
    else if (!topic_restriction)
        this->topic = topic;
}

void Channel::set_password(const std::string &password, Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        this->password = password;
}

void Channel::add_member(const Client& new_client) {
	if (find(member.begin(), member.end(), new_client) == member.end())
	    this->member.push_back(new_client);
}

void Channel::add_operator(const Client& new_operator, Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        this->operator_member.push_back(new_operator);
}

void Channel::set_invit_only(Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        invit_only = true;
}

void Channel::set_invit_open(Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        invit_only = false;
}

void Channel::set_topic_restricted(Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        topic_restriction = true;
}

void Channel::set_topic_not_restricted(Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        topic_restriction = false;
}

void Channel::set_user_limit(int new_user_limit, Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        user_limit = new_user_limit;
}

std::string Channel::get_name() {
    return this->name;
}

std::list<Client>::iterator Channel::get_member_begin() {
    return this->member.begin();
}

std::list<Client>::iterator Channel::get_member_end() {
    return this->member.end();
}
