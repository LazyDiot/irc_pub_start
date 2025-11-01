#include "Channels.hpp"
#include "Users.hpp"

#include <iostream>
#include <string.h>

class user;
class channel;

int is_command(char *s)
{
    std::string cmd = s;
    if (cmd == "/KICK")
        return (1);
    else if (cmd == "/INVITE")
        return (2);
    else if (cmd == "/TOPIC")
        return (3);
    else if (cmd == "/MODE")
        return (4);
    else if (cmd == "/JOIN")
        return (5);
    else if (cmd == "/LEAVE")
        return (6);
    else if (cmd == "/NAMES")
        return (7);
    else if (cmd == "/CREATE")
        return (8);
    else
        return (0); //assumes "/MSG" ET tout autre input garbage
}

void channel::kick(std::string &s, user *send)
{
    char *change, split[s.size() + 1], *reciever;
    strncpy(split, s.c_str(), s.size() + 1);
    split[s.size()] = 0;//merci les atrocités que le c++ me force à commettre
    std::string rec;

    reciever = strtok(split, " ");
    reciever = strtok(NULL, " ");
    if (!reciever) // i.e. si la personne a juste écrit "KICK"
    {
        std::cout << "Error executing command : \"KICK\". Expected syntax : /KICK <username>\n" << std::endl;
        return ;
    }
    this->kick_user(send, rec = reciever);
}

void channel::invite(std::string &s, user *send)
{
    char *change, split[s.size() + 1], *reciever;
    strncpy(split, s.c_str(), s.size() + 1);
    split[s.size()] = 0;

    reciever = strtok(split, " ");
    reciever = strtok(NULL, " ");
    if (!reciever) // idem
    {
        std::cout << "Error executing command : \"INVITE\". Expected syntax : /INVITE <username>\n" << std::endl;
        return ;
    }
    this->invite_user(send, reciever);
}

void channel::topic(std::string &s, user *send)
{
    char *topic, split[s.size() + 1];
    strncpy(split, s.c_str(), s.size() + 1);
    split[s.size()] = 0;
    std::string subject;

    topic = strtok(split, " ");
    topic = strtok(NULL, " ");
    if (!topic)
    {
        std::cout << "Error executing command : \"TOPIC\". Expected syntax : /TOPIC <subject>\n" << std::endl;
        return ;
    }
    subject = topic;
    this->change_topic(send, subject);
    this->broadcast_message("The topic of this channel has been changed by " + send->getNick() + " to " + subject + ".\n");
}

ssize_t cpp_string_to_ssizet(std::string &s)
{
    if (!s.size())
        return (-1);
    std::stringstream ss;
    ssize_t i;
    ss << s;
    ss >> i;
    return (i);
}

void channel::mode(std::string &s, user *send) //tres tres TRES sale. sorry
{
    //MODE ±i; ±t; ±k; ±o; ±l? ou juste la lettre et ca swap on/off?
    const char *split1 = s.c_str();
    char *change, split[s.size() + 1], *add_on;
    strncpy(split, s.c_str(), s.size() + 1);
    split[s.size()] = 0;
    change = strtok(split, " ");//"MODE"
    change = strtok(NULL, " ");//faut check 11 possibilités donc ca va forcément être dégueu? pas de pointeur sur fn car args differents
    if (!change)
        return (send->recieve_message("Error executing \"/MODE\" command. Expected syntax : /MODE <option> (one of the following : ±i; ±t; ±k; ±o; ±l?) <target/limit>.\n"));
    std::string mode = change, additional;
    if (mode == "-i")
        channel::lift_invite_restriction(send);
    else if (mode == "+i")
        channel::enforce_invite_restriction(send);
    else if (mode == "-t")
        channel::lift_topic_restriction(send);
    else if (mode == "+t")
        channel::enforce_topic_restriction(send);
    else if (mode == "-k")
        channel::lift_password_restriction(send);
    else if (mode == "+k")
    {
        add_on = strtok(NULL, " ");
        if (!add_on)
            return (send->recieve_message("Error executing \"/MODE\" password command. Expected syntax : /MODE +k <password>\n"));
        channel::enforce_password_restriction(send, additional = add_on);
    }
    else if (mode == "-o")
    {
        add_on = strtok(NULL, " ");
        if (!add_on)
            return (send->recieve_message("Error executing \"/MODE\" operator command. Expected syntax : /MODE -o <username>\n"));
        channel::demote(send, additional = add_on);
    }
    else if (mode == "+o")
    {
        add_on = strtok(NULL, " ");
        if (!add_on)
            return (send->recieve_message("Error executing \"/MODE\" operator command. Expected syntax : /MODE +o <username>\n"));
        channel::promote_to_op(send, additional = add_on);
    }
    else if (mode == "-l")
        channel::lift_user_limit(send);
    else if (mode == "+l")
    {
        add_on = strtok(NULL, " ");
        if (!add_on)
            return (send->recieve_message("Error executing \"/MODE\" user limit command. Expected syntax : /MODE +l <limit>\n"));
        channel::enforce_user_limit(send, cpp_string_to_ssizet(additional = add_on));
    }
    else 
        return (send->recieve_message("unrecognized MODE command. expected syntax : <+> or <->, directly followed by either <i>, <t>, <k>, <o> or <l> (example : MODE +k <password>) \n"));
}

void channel::join(std::string &chann, user *send)//password? on prompt le user quand il join ig? à checker, manque le pass ou non
{
    if (chann.find("/JOIN") + 1 >= chann.size())
        return (send->recieve_message("Error with /JOIN command. Please specify what channel you wish to join (one accepted only for each command).\n"));
    std::string check = chann.substr(chann.find("/JOIN ")), cchan;

    char split[check.size() + 1], *subchan;
    strncpy(split, check.c_str(), check.size() + 1);
    subchan = strtok(split, " ");//pas besoin de check NULL vu que la premiere ligne de la fonction check la taille
    channel *chan = serv->getCorrectChannel(cchan = subchan);
    if (!chan)
        return (send->recieve_message("Said channel doesn't exist. Please check for typos\n"));
    std::vector<user *>::iterator it = chan->pv_users_in_chann.begin(), last = chan->pv_users_in_chann.end();
    while (it != last)
	{
		if (*it == send) //si déjà dans le chann
			return (send->recieve_message("You already joined channel " + cchan + "\n"));
		it ++;
	}
    if ((!chan->getInviteStatus() && !chan->getPasswordStatus()) || send->getAdmin()) //si admin ou free-access chann
    {
        chan->broadcast_message(send->getNick() + " joined channel " + cchan + "\n");
        chan->pv_users_in_chann.push_back(send);
        return (send->recieve_message("You have joined " + cchan + "\n"));
    }
    else //soit invite only, soit pw, soit les deux; une seule condition necessaire pour entrer
    {
        it = chan->pv_whitelist.begin(), last = chan->pv_whitelist.end();
        while (it != last)
        {
            if (*it == send)
            {
                chan->broadcast_message(send->getNick() + " joined channel " + cchan + "\n");
                chan->pv_users_in_chann.push_back(send);
                return (send->recieve_message("You have joined " + cchan + "\n"));
            }
            it ++;
        }
        subchan = strtok(NULL, " ");
        if (!subchan)
            return (send->recieve_message("You didn't join " + cchan + ". It requires a non-empty password to join.\n"));
        if (subchan == chan->getPassword())
        {
            chan->pv_users_in_chann.push_back(send);
            chan->broadcast_message(send->getNick() + " joined channel " + chan->getName() + "\n");
            return (send->recieve_message("You have joined " + chan->getName() + "\n"));
        }
        return (send->recieve_message("Wrong password. you didn't join " + chan->getName() + "\n"));
    }
}

void channel::leave(std::string &chann, user *send)//changer le last_room
{
    if (chann.find("/LEAVE") + 1 >= chann.size())
        return (send->recieve_message("Error with /LEAVE command. Please specifiy what channel you want to leave (one accepted only for each command).\n"));
    std::string check = chann.substr(chann.find("/LEAVE "));
    channel *chan = serv->getCorrectChannel(check);
    if (!chan)
        return (send->recieve_message("Said channel doesn't exist. Please check for typos\n"));
    std::vector<user *>::iterator it = chan->pv_chann_modos.begin(), last = chan->pv_chann_modos.end();
	while (it != last)
	{
		if (*it == send)
        {
            delete *it;
			break ;
        }
		it ++;
	}
    it = chan->pv_users_in_chann.begin(), last = chan->pv_users_in_chann.end();
    while (it != last)
	{
		if (*it == send)
        {
            delete *it;
            send->reset_room();
			break ;
        }
		it ++;
	}
    if (chan->pv_users_in_chann.empty())//ferme le chann si vide
        return (chan->close());
    if (chan->pv_chann_modos.empty())//transfere les privilèges modos si le last a leave (y'a au moins un user dans le chann vu qu'on a check avant)
    {
        it = chan->pv_users_in_chann.begin() + 1;
        chan->pv_chann_modos.push_back(*it);
        user *u = *it;
        u->recieve_message("Last moderator left the channel, granting you these privileges.\n");
    }
    chan->broadcast_message(send->getNick() + " left " + this->getName());
}

void channel::check_and_send(user *send)
{
    std::vector<user *>::iterator it = pv_users_in_chann.begin(), last = pv_users_in_chann.end();
    while (it != last)
    {
        if (*it == send)
            return (send->recieve_message(show_users()));
        it ++;
    }
}

void channel::display(std::string &s, user *send) // /NAMES
{
    char test[s.size() + 1];
    strncpy(test, s.c_str(), s.size() + 1);
    test[s.size()] = 0; //sécurité
    char *precision = strtok(test, " "), *chan_to_check;
    precision = strtok(NULL, " ");
    std::string check_chan;
    if (!(precision)) // i.e. juste tapé "/NAMES"
    {
        if (send->getlast_room() == "")
            return (send->recieve_message("No active channel found.\n"));
        return (send->recieve_message(this->show_users()));
    }
    channel *chan;
    chan_to_check = strtok(NULL, " ");
    if (!(chan_to_check))
    {
        if (precision == (std::string)"-count")
            return (send->recieve_message(cpp_ssizet_to_string(count_users())));
        else if (precision == (std::string)"-ops")
            return (send->recieve_message(show_mods()));
        else if (precision == (std::string)"**")
            return (serv->all_channs(send));
        else
            return (send->recieve_message("Unrecognized syntax " + (std::string)precision + " .\n"));
    }
    else
    {
        while (chan_to_check)
        {
            if (chan_to_check[0] != '#')
                send->recieve_message("Unrecognized channel syntax -> " + (std::string)chan_to_check + " expected : <#channel name> .\n");
            else
            {
                check_chan = chan_to_check;
                check_chan.erase(check_chan.begin());//vire le '#'
                chan = serv->getCorrectChannel(check_chan);
                if (!chan)
                    send->recieve_message("Unrecognized channel name -> " + check_chan + " please check for typos .\n");
                else
                {
                    if (precision == (std::string)"-count")
                        send->recieve_message(cpp_ssizet_to_string(chan->count_users()));
                    else if (precision == (std::string)"-ops")
                        send->recieve_message(chan->show_mods());
                    // else
                    //     send->recieve_message(chan->show_users());
                }
            }
            chan_to_check = strtok(NULL, ",");
        }
    }
}

void channel::check_name(std::string &s, user *send)
{
    std::string name = s.substr(s.find("/CREATE") + 1);
    serv->create_channel(send, name);
}

void channel::parse_string(std::string &s, user *send)
{
    char test[s.size() + 1];
    strncpy(test, s.c_str(), s.size() + 1);
    test[s.size()] = 0; //sécurité
    char *cmd = strtok(test, " ");
    void (channel::*execute_command[])(std::string &, user *) = {&channel::analyse_msg_content, &channel::kick, &channel::invite, &channel::topic, &channel::mode, &channel::join, &channel::leave, &channel::display, &channel::check_name};
    (this->*execute_command[is_command(cmd)])(s, send);
}
