#ifndef SERVER_HPP
# define SERVER_HPP

# define MAX_USERS_IN_CHAT 150
# define MSG_SIZE_MAX 255
# define NAME_SIZE_MAX 25

# include <iostream>
# include <vector>
# include <sstream>
# include "Users.hpp"
# include "Channels.hpp"
# include <sys/types.h>

class user;
class channel;

class server {
        unsigned int            pv_port;
        std::string             pv_name;
        std::vector<user *>     pv_user_list;
        std::vector<channel *>  pv_chann_list;
        std::string             pv_basic_pw; //strings ou char*? vu que ca doit être comparé à du argv?
        std::string             pv_admin_pw;
    public : 
        bool duplicate_channelname(std::string);
        bool duplicate_username(std::string);
        bool duplicate_nickname(std::string);

        channel *getCorrectChannel(std::string &);
        user *getCorrectUser(std::string &name);
        void all_channs(user *send);

        void create_channel(user *u, std::string name);
        void create_lobby();
};

#endif