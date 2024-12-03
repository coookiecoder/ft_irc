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

std::string Channel::set_topic(const std::string &topic, Client& user) {
    if (topic_restriction && find(operator_member.begin(), operator_member.end(), user) != operator_member.end()) {
        this->topic = topic;
		size_t pos = topic.find(':');
		std::string message_to_client = topic.substr(pos + 1);
		return std::string(":" + user.get_nick() + "!" + user.get_user() + "@" + user.get_host() + " TOPIC " + name + " :" + message_to_client + "\n");
	} else if (!topic_restriction) {
        this->topic = topic;
		size_t pos = topic.find(':');
		std::string message_to_client = topic.substr(pos + 1);
		return std::string(":" + user.get_nick() + "!" + user.get_user() + "@" + user.get_host() + " TOPIC " + name + " :" + message_to_client + "\n");
	}
	return std::string("");
}

void Channel::set_password(const std::string &password, Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        this->password = password;
}

bool Channel::add_member(const Client& new_client, std::string password) {
	if (find(member.begin(), member.end(), new_client) == member.end() && (password == this->password || this->password.empty()) && !invite_only) {
		this->member.push_back(new_client);
		return true;
	} else if (find(member.begin(), member.end(), new_client) == member.end() && find(invinted_member.begin(), invinted_member.end(), new_client) != invinted_member.end()) {
		this->member.push_back(new_client);
		return true;
	}
	return false;
}

void Channel::add_operator(const Client& new_operator, Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        this->operator_member.push_back(new_operator);
}

void Channel::remove_operator(Client& new_operator, Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end()) {
		for (std::list<Client>::iterator client_list = this->operator_member.begin(); client_list != this->operator_member.end(); client_list++) {
			if (client_list->get_nick() == new_operator.get_nick())
				this->operator_member.remove(*client_list);
		}
	}
}

void Channel::add_invinted(const Client& new_client, Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
		if (find(invinted_member.begin(), invinted_member.end(), user) == invinted_member.end())
	        this->invinted_member.push_back(new_client);
}

void Channel::set_invit_only(Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        invite_only = true;
}

void Channel::set_invit_open(Client& user) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end())
        invite_only = false;
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

void Channel::set_mode(std::string message, Client& user) {
    std::istringstream token(message);
    std::string modeStr;
    token >> modeStr;
	token >> modeStr;
	token >> modeStr;

    if (modeStr.size() < 2)
        return;

    char mode = modeStr[1];
    bool addingMode = (modeStr[0] != '-');  // '+' for adding modes, '-' for removing

	std::string nick;
	std::string password;
	int user_limit;

    switch (mode) {
        case 'i':
            if (addingMode)
				set_invit_only(user);
            else
                set_invit_open(user);
            break;
        case 't':
            if (addingMode)
				set_topic_restricted(user);
			else
				set_topic_not_restricted(user);
            break;
        case 'k':
			token >> password;
            if (addingMode)
				set_password(password, user);
			else
				set_password("", user);
            break;
        case 'o':
            token >> nick;
            if (addingMode) {
				for (std::list<Client>::iterator client_list = this->member.begin(); client_list != this->member.end(); client_list++) {
					if (client_list->get_nick() == nick)
						this->add_operator(*client_list, user);
				}
			} else {
				for (std::list<Client>::iterator client_list = this->operator_member.begin(); client_list != this->operator_member.end(); client_list++) {
					if (client_list->get_nick() == nick)
						this->remove_operator(*client_list, user);
				}
			}
            break;
        case 'l':
			token >> user_limit;
            if (addingMode)
				set_user_limit(user_limit, user);
			else
                set_user_limit(0, user);
            break;
        default:
            break;
    }
}

bool Channel::is_member(int client_fd) {
	for (std::list<Client>::iterator client_list = this->member.begin(); client_list != this->member.end(); client_list++) {
		if (client_list->get_fd() == client_fd)
			return (true);
	}
	return (false);
}

void Channel::kick(std::string nick_to_kick, Client &user, std::string message) {
    if (find(operator_member.begin(), operator_member.end(), user) != operator_member.end()) {
		for (std::list<Client>::iterator client_list = this->member.begin(); client_list != this->member.end(); client_list++) {
			if (client_list->get_nick() == nick_to_kick) {
				size_t pos = message.find(':');
				std::string message_to_client = message.substr(pos + 1);
				std::string buffer(":" + user.get_nick() + "!" + user.get_user() + "@" + user.get_host() + " KICK " + name + " " + client_list->get_nick() + " :" + message_to_client + "\n");
				send(client_list->get_fd(), buffer.c_str(), buffer.length(), MSG_DONTWAIT);
				member.remove(*client_list);
				return ;
			}
		}
	}
}

void Channel::del_user(Client& user) {
	for (std::list<Client>::iterator client_list = this->member.begin(); client_list != this->member.end(); client_list++) {
		if (client_list->get_fd() == user.get_fd()) {
			member.remove(*client_list);
			break;
		}
	}

	for (std::list<Client>::iterator client_list = this->operator_member.begin(); client_list != this->operator_member.end(); client_list++) {
		if (client_list->get_fd() == user.get_fd()) {
			operator_member.remove(*client_list);
			break;
		}
	}

	for (std::list<Client>::iterator client_list = this->invinted_member.begin(); client_list != this->invinted_member.end(); client_list++) {
		if (client_list->get_fd() == user.get_fd()) {
			invinted_member.remove(*client_list);
			break;
		}
	}
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
