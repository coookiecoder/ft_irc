#pragma once

#include <list>
#include <string>
#include <algorithm>

#include <Client.hpp>

class Channel {
    private:
        std::string name;
        std::string topic;
        std::string password;
        std::list<Client> member;
        std::list<Client> operator_member;

        bool invit_only;
        bool topic_restriction;
        int user_limit;
    public:
        Channel(const std::string &name, Client &user);
        ~Channel();

        void set_name(const std::string &name);
        void set_topic(const std::string &topic, Client& user);
        void set_password(const std::string &password, Client& user);

        void add_member(const Client& new_client);
        void add_operator(const Client& new_operator, Client& user);

        void set_invit_only(Client& user);
        void set_invit_open(Client& user);

        void set_topic_restricted(Client& user);
        void set_topic_not_restricted(Client& user);

        void set_user_limit(int new_user_limit, Client& user);

        std::string get_name();

        std::list<Client>::iterator get_member_begin();
        std::list<Client>::iterator get_member_end();
};
