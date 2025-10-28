#include "Users.hpp"
#include "Server.hpp"

user::user(int id)
{
    pv_nick = "";
    pv_username = "";
    pv_id = id;
    pv_isadmin = false;
    last_reciever = "";
    last_sender = "";
    last_room = "";
}

user::~user()
{
    pv_msgqueue.clear();
}

void user::check_send(std::string s, server *serv)
{
    user *u = serv->getCorrectUser(last_sender);
    if (!u)
    {
        this->recieve_message("Can't send message to " + s + " . User no longer is connected.\n");
        this->last_sender = "";
    }
    else
        this->send_message(s, *u);
}

void user::check_rec(std::string s, server *serv)
{
    user *u = serv->getCorrectUser(last_reciever);
    if (!u)
    {
        this->recieve_message("Can't send message to " + s + " . User no longer is connected.\n");
        this->last_reciever = "";
    }
    else
        this->send_message(s, *u);
}

void user::send_message(const std::string &msg, user &dest) 
{
    dest.recieve_message(msg);
    dest.set_sender(this);
    this->last_reciever = dest.getNick();
}


void user::recieve_message(const std::string &msg) 
{
    pv_msgqueue.push_back(msg);
    //c'est ici que je fais un check random pour tout afficher si c'est okay?
    //donc je fais un check et je call empty_queue?
}

void user::empty_queue()
{
    std::vector<std::string>::iterator it = pv_msgqueue.begin(), last = pv_msgqueue.end();
    while (it != last)
    {
        //afficher le message dans le fd correspondant au user? on fait comment?
        pv_msgqueue.erase(it);
        it ++; //probleme de ToC/ToU avec le .end?
        last = pv_msgqueue.end();
    }
}

bool user::getAdmin() const
{
    return (pv_isadmin);
}

int user::getID() const
{
    return (pv_id);
}

std::string user::getNick() const
{
    return (pv_nick);
}

std::string user::getUserName() const
{
    return (pv_username);
}

std::string user::getlast_room() const
{
    return (this->last_room);
}
std::string user::getlastrec() const
{
    return (this->last_reciever);
}

std::string user::getlastsend() const
{
    return (this->last_sender);
}

void user::set_reciever(user *rec)
{
    last_reciever = rec->getNick();
}

void user::set_room(channel &chan)
{
    last_room = chan.getName();
}

void user::setNick(const std::string &nick)
{
    this->pv_nick = nick;
}

void user::setUserName(const std::string &username)
{
    this->pv_username = username;
}

void user::set_sender(user *sender)
{
    this->last_sender = sender->getNick();
}
