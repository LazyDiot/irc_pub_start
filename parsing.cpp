#include "Channels.hpp"
#include "Users.hpp"

#include <iostream>
#include <string.h>

class user;
class channel;

//if you want to see real talent... then fucking go somewhere else

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
    else
        return (0); //assumes "/MSG ET tout autre input garbage"
}

void channel::kick(std::string &s, user *send)
{
    char *change, split[s.size()], *reciever;
    strncpy(split, s.c_str(), s.size() + 1);
    if (!split)
        return (send->recieve_message("Error processing your message. Please try again\n"));
    split[s.size()] = 0;//merci les atrocités que le c++ me force à commettre
    std::string reason, rec;

    reciever = strtok(split, " ");
    reciever = strtok(NULL, " ");
    if (!reciever) // i.e. si la personne a juste écrit "KICK"
    {
        std::cout << "Error executing command : \"KICK\". Expected syntax : /KICK <username>\n" << std::endl;
        return ;
    }
    reason = split;
    this->kick_user(send, rec = reciever /*reason*/);
}

void channel::invite(std::string &s, user *send)
{
    char *change, split[s.size()], *reciever;
    strncpy(split, s.c_str(), s.size() + 1);
    if (!split)
        return (send->recieve_message("Error processing your message. Please try again\n"));
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
    char *topic, split[s.size()];
    strncpy(split, s.c_str(), s.size() + 1);
    if (!split)
        return (send->recieve_message("Error processing your message. Please try again\n"));
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
    char *change, split[s.size()], *add_on;
    strncpy(split, s.c_str(), s.size() + 1);
    if (!split)
        return (send->recieve_message("Error processing your message. Please try again\n"));
    split[s.size()] = 0;
    change = strtok(split, " ");//"MODE"
    change = strtok(NULL, " ");//faut check 11 possibilités donc ca va forcément être dégueu? pas de pointeur sur fn car args differents
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
        channel::enforce_password_restriction(send, additional = add_on);
    }
    else if (mode == "-o")
    {
        add_on = strtok(NULL, " ");
        channel::demote(send, additional = add_on);
    }
    else if (mode == "+o")
    {
        add_on = strtok(NULL, " ");
        channel::promote_to_op(send, additional = add_on);
    }
    else if (mode == "-l")
        channel::lift_user_limit(send);
    else if (mode == "+l")
    {
        add_on = strtok(NULL, " ");
        channel::enforce_user_limit(send, cpp_string_to_ssizet(additional = add_on));
    }
    else 
        return (send->recieve_message("unrecognized MODE command. expected syntax : <+> or <->, directly followed by either <i>, <t>, <k>, <o> or <l> (example : MODE +k <password>) \n"));
}

void channel::join(std::string &chann, user *send)//password? on prompt le user quand il join ig? pareil, comment check si le chann existe?
{
    channel *chan = serv->getCorrectChannel(chann);
    if (!chan)
        return (send->recieve_message("Said channel doesn't exist. Please check for typos\n"));
    std::vector<user *>::iterator it = chan->pv_users_in_chann.begin(), last = chan->pv_users_in_chann.end();
    while (it != last)
	{
		if (*it == send) //si déjà dans le chann
			return (send->recieve_message("You already joined channel " + chann + "\n"));
		it ++;
	}
    if ((!chan->getInviteStatus() && !chan->getPasswordStatus()) || send->getAdmin()) //si admin ou free-access chann
    {
        chan->broadcast_message(send->getNick() + " joined channel " + chan->getName() + "\n");
        chan->pv_users_in_chann.push_back(send);
        return (send->recieve_message("You have joined " + chan->getName() + "\n"));
    }
    else if (chan->getInviteStatus() && !chan->getPasswordStatus()) //si invite only SANS password, potentiellement supprimer la condition password sinon merge les deux
    {
        it = chan->pv_whitelist.begin(), last = chan->pv_whitelist.end();
        while (it != last)
        {
            if (*it == send)
            {
                chan->broadcast_message(send->getNick() + " joined channel " + chan->getName() + "\n");
                chan->pv_users_in_chann.push_back(send);
                return (send->recieve_message("You have joined " + chan->getName() + "\n"));
            }
            it ++;
        }
        return (send->recieve_message("This channel ( " + chan->getName() + " ) is in invite-only mode. You needed to be invited my a moderator or an admin.\n"));
    }
    // else if (chan->getInviteStatus() && chan->getPasswordStatus()) // si invite only ET password (stupide nan? si c'est invite-only pas besoin du password?)
    else if (chan->getPasswordStatus())
    {
        send->recieve_message("This channel ( " + chan->getName() + " ) requires a password. Please enter it\n");
        std::string input;
        std::getline(std::cin, input);
        if (input == chan->getPassword())
        {
            chan->broadcast_message(send->getNick() + " joined channel " + chan->getName() + "\n");
            chan->pv_users_in_chann.push_back(send);
            return (send->recieve_message("You have joined " + chan->getName() + "\n"));
        }
        return (send->recieve_message("Wrong password. you didn't join " + chan->getName() + "\n"));
    }
}

void channel::leave(std::string &chann, user *send)//comment je check si le user est bien dans le chann en question?
{
    channel *chan = serv->getCorrectChannel(chann);
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

void channel::parse_string(std::string &s, user *send)
{
    char test[s.size()];
    strncpy(test, s.c_str(), s.size() + 1);
    if (!test)
        return (send->recieve_message("Error processing your message. Please try again\n"));
    test[s.size()] = 0; //sécurité
    char *cmd = strtok(test, " ");
    void (channel::*execute_command[])(std::string &, user *) = {&channel::analyse_msg_content, &channel::kick, &channel::invite, &channel::topic, &channel::mode};
    (this->*execute_command[is_command(cmd)])(s, send);
}
