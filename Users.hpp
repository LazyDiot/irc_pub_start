#ifndef USERS_HPP
# define USERS_HPP

# include <iostream>
# include <vector>
# include <sstream>

class user{
        int                         pv_id;
        std::string                 pv_nick;
        std::string                 pv_username;
        bool                        pv_isadmin;
        std::vector<std::string>    pv_msgqueue;
        //nick et username à la construction, ou plus tard?
        //authenticate?
    public:
        user(int id);
        ~user();
        
        void send_message(const std::string &msg, user &dest);
        void recieve_message(const std::string &msg);//ouais c'est bien beau mais, je fais comment pour les afficher? un vieux while(1) dégueu?
        void empty_queue();

        bool getAdmin() const;
        int getID() const;

        void setNick(const std::string &);
        void setUserName(const std::string &);
        std::string getNick() const;
        std::string getUserName() const;

};

#endif