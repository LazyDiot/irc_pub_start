#include "Server.hpp"

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