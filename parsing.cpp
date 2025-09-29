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
    else
        return (0);
}

void channel::kick(std::string &s, user *send)
{
    char *change, split[s.size() + 1], *reciever;
    strncpy(split, s.c_str(), s.size());
    if (!split)
        return (send->recieve_message("Error processing your message. Please try again\n"));
    split[s.size() + 1] = '\0';//merci les atrocités que le c++ me force à commettre
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
    char *change, split[s.size() + 1], *reciever;
    strncpy(split, s.c_str(), s.size());
    if (!split)
        return (send->recieve_message("Error processing your message. Please try again\n"));
    split[s.size() + 1] = '\0';

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
    strncpy(split, s.c_str(), s.size());
    if (!split)
        return (send->recieve_message("Error processing your message. Please try again\n"));
    split[s.size() + 1] = '\0';
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
    char *change, split[s.size() + 1], *add_on;
    strncpy(split, s.c_str(), s.size());
    if (!split)
        return (send->recieve_message("Error processing your message. Please try again\n"));
    split[s.size() + 1] = '\0';
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


//useless ou pas? https://irssi.org/New-users/
void channel::parse_string(std::string &s, user *send)
{
    char test[s.size() + 1];
    strncpy(test, s.c_str(), s.size());
    if (!test)
        return (send->recieve_message("Error processing your message. Please try again\n"));
    test[s.size() + 1] = '\0';
    char *cmd = strtok(test, " ");
    void (channel::*execute_command[])(std::string &, user *) = {&channel::broadcast_message, &channel::kick, &channel::invite, &channel::topic, &channel::mode};
    (this->*execute_command[is_command(cmd)])(s, send);
}

// https://irssi.org/documentation/help/msg/
// donc ce qui est au dessus est incomplet-> manque un send en privé