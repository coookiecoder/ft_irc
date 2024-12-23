#pragma once

#include <list>
#include <string>
#include <algorithm>

#include <sys/socket.h>

#include <Client.hpp>

class Channel {
    private:
        std::string name;
        std::string topic;
        std::string password;
        std::list<Client> member;
        std::list<Client> operator_member;
        std::list<Client> invinted_member;

        bool invite_only;
        bool topic_restriction;
        int user_limit;
		int user;
    public:
        Channel(const std::string &name, Client &user);
        ~Channel();

        void set_name(const std::string &name);
		std::string set_topic(const std::string &topic, Client& user);
        void set_password(const std::string &password, Client& user);

        bool add_member(const Client& new_client, std::string password);
        void add_operator(Client& new_operator, Client& user);
		void remove_operator(Client& new_operator, Client& user);
		void add_invinted(const Client& new_client, Client& user);

        void set_invit_only(Client& user);
        void set_invit_open(Client& user);

        void set_topic_restricted(Client& user);
        void set_topic_not_restricted(Client& user);

        void set_user_limit(int new_user_limit, Client& user);

		void set_mode(std::string message, Client& user);

		bool is_member(int client_fd);
	
		void kick(std::string nick_to_kick, Client &user, std::string message);

		void del_user(Client& user);

        std::string get_name();

        std::list<Client>::iterator get_member_begin();
        std::list<Client>::iterator get_member_end();
};
