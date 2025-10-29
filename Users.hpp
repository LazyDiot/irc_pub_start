#ifndef USERS_HPP
# define USERS_HPP

# include <iostream>
# include <vector>
# include <sstream>
# include "Channels.hpp"
# include "Server.hpp"
# include <sys/types.h>

class channel;
class server;

class user{
        int                         pv_id;
        std::string                 pv_nick;
        std::string                 pv_username;
        bool                        pv_isadmin;
        std::vector<std::string>    pv_msgqueue;
        std::string                 last_sender;
        std::string                 last_reciever;
        std::string                 last_room;
        //nick et username à la construction, ou plus tard?
        //authenticate?
    public:
        user(int id);
        ~user();
        
        void send_message(const std::string &msg, user &dest);
        void check_rec(std::string , server *);
        void check_send(std::string , server *);
        void recieve_message(const std::string &msg);//ouais c'est bien beau mais, je fais comment pour les afficher? un vieux while(1) dégueu?
        void empty_queue();

        bool getAdmin() const;
        int getID() const;

        void setNick(const std::string &);
        void setUserName(const std::string &);
        void set_sender(user *);
        void set_reciever(user *);
        void set_room(channel &);
        void reset_room();
        std::string getNick() const;
        std::string getUserName() const;
        std::string getlast_room() const;
        std::string getlastrec() const;
        std::string getlastsend() const;

};

#endif