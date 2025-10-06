#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include <sstream>
# include "Users.hpp"
# include "Channels.hpp"

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

        channel *getCorrectChannel(std::string &);
};

#endif