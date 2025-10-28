#include "Server.hpp"

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