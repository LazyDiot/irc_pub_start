#include "Server.hpp"

void server::create_channel(user *u, std::string name)
{
    if (!(duplicate_channelname(name)))
    {
        channel chan(name);
        this->pv_chann_list.push_back(&chan);
        chan.add_creator(u);
        u->recieve_message("You created " + name + " channel. You are granted moderator privileges in it.\n");
    }
    else
        u->recieve_message("A channel with this name \"" + name + "\" already exists.\n");
}

void server::create_lobby()
{
    channel chan((std::string)"lobby");
    pv_chann_list.push_back(&chan);
}

bool server::duplicate_nickname(std::string name)
{
    user *u;
    std::vector<user *>::iterator it = pv_user_list.begin(), last = pv_user_list.end();
    while (it != last)
    {
        u = *it;
        if (u->getNick() == name)
            return (true);
        it ++;
    }
    return (false);
}

bool server::duplicate_username(std::string name)
{
    user *u;
    std::vector<user *>::iterator it = pv_user_list.begin(), last = pv_user_list.end();
    while (it != last)
    {
        u = *it;
        if (u->getUserName() == name)
            return (true);
        it ++;
    }
    return (false);
}

user *server::getCorrectUser(std::string &name)
{
    user *ret;

    std::vector<user *>::iterator it = pv_user_list.begin(), last = pv_user_list.end();
    while (it != last)
    {
        ret = *it;
        if (ret->getNick() == name)
            return (ret);
        it ++;
    }
    return (NULL);
}

bool server::duplicate_channelname(std::string name)
{
    channel *chan;
    std::vector<channel *>::iterator it = pv_chann_list.begin(), last = pv_chann_list.end();
    while (it != last)
    {
        chan = *it;
        if (chan->getName() == name)
            return (true);
        it ++;
    }
    return (false);
}

channel *server::getCorrectChannel(std::string &name)
{
    channel *ret;

    std::vector<channel *>::iterator it = pv_chann_list.begin(), last = pv_chann_list.end();
    while (it != last)
    {
        ret = *it;
        if (ret->getName() == name)
            return (ret);
        it ++;
    }
    return (NULL);
}

void server::all_channs(user *send)
{
    std::vector<channel *>::iterator fchan = pv_chann_list.begin(), lchan = pv_chann_list.end();
    channel *chan;
    while (fchan != lchan)
    {
        chan = *fchan;
        chan->check_and_send(send);
        fchan ++;
    }
}