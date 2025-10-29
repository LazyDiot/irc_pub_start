#include "Channels.hpp"
#include "Users.hpp"

#include <iostream>
#include <string.h>

channel::channel(std::string name)
{
    pv_name = name;
	pv_password = "";
	pv_topic = "";
	pv_invite_only = false;
	pv_topic_op_restricted = false;
	pv_needs_password = false;
	pv_user_limit = -1;
}

channel::~channel()
{
    pv_users_in_chann.clear();
    pv_chann_modos.clear();
    pv_whitelist.clear();
}

void channel::close()
{
	this->~channel();
}

void channel::broadcast_message(const std::string &msg) 
{
    std::vector<user *>::iterator it = pv_users_in_chann.begin(), last = pv_users_in_chann.end();
    while (it != last)
    {
		user *test = *it;
        test->recieve_message(msg); //mettre le sender du message via un getnick
        it ++;
        //pas besoin de refresh last, un message balancé avant que quelqu'un arrive ne devrait pas être reçu par ce quelqu'un
    }
}

void channel::broadcast_message(std::string msg, user *sender)
{
    std::vector<user *>::iterator it = pv_users_in_chann.begin(), last = pv_users_in_chann.end();
    while (it != last)
    {
		if (*it == sender)
		{
			it ++;
			continue;
		}
		user *test = *it;
        test->recieve_message(msg); //mettre le sender du message via un getnick
        it ++;
        //same
    }
}

void channel::warn_user(std::string &msg, user *sender)
{
	sender->recieve_message("Unrecognized syntax : -> \"" + msg + "\".\n");
}

void channel::analyse_msg_content(std::string &msg, user *send)//changer le last_room et les last_users /MSG
{
	char split[msg.size() + 1], *target;
    strncpy(split, msg.c_str(), msg.size() + 1);
    if (!split)
        return (send->recieve_message("Error processing your message. Please try again\n"));
    split[msg.size()] = 0;

    target = strtok(split, " ");// /MSG
	target = strtok(NULL, " ");
	std::string check;
	channel *chan;
	user *u;
	if (!target)
		return (send->recieve_message("Error with /MSG command. Expected syntax : \" /MSG <#channel or nickname> message \" for example."));
	else if (target == (std::string)"*")
	{
		if (msg.find('*') + 2  > msg.size())
			return (send->recieve_message("Error with /MSG * command. Message needed.\n"));
		return (broadcast_message(msg.substr(msg.find('*') + 2), send));
	}
	else if (target == (std::string)".")
	{
		if (msg.find('.') + 2 > msg.size())
			return (send->recieve_message("Error with /MSG . command. Message needed.\n"));
		if (send->getlastrec() == "")
			return (send->recieve_message("Error with /MSG . command. Nobody to send message to.\n"));
		return (send->check_rec(msg.substr(msg.find('*') + 2), this->serv));
	}
	else if (target == (std::string)",")
	{
		if (msg.find(',') + 2 > msg.size())
			return (send->recieve_message("Error with /MSG , command. Message needed.\n"));
		if (send->getlastsend() == "")
			return (send->recieve_message("Error with /MSG , command. Nobody to send message to.\n"));
		return (send->check_send(msg.substr(msg.find('*') + 2), this->serv));
	}
	else if (target[0] == '#')
	{
		check = target;
		check.erase(check.begin());
		if (!(chan = serv->getCorrectChannel(check)))
			return (send->recieve_message("Error with /MSG # command. Channel not found.\n"));
		send->set_room(*chan);
		return (chan->broadcast_message(msg.substr(msg.find(check) + 1), send));
	}
	else
	{
		check = target;
		if (!(u = serv->getCorrectUser(check)))
			return (send->recieve_message("Error with /MSG commande. User " + check + " not found.\n"));
		return (send->send_message(msg.substr(msg.find(check) + 1), *u));
	}
}

void channel::add_creator(user *u)
{
	this->pv_chann_modos.push_back(u);
	this->pv_users_in_chann.push_back(u);
}

void channel::change_topic(user *sender, std::string &tp)
{
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	if (this->pv_topic_op_restricted)
	{
		if (sender->getAdmin())
			goto topic_change;
		while (it != last)
		{
			if (*it == sender)
				goto topic_change ;
			it ++;
		}
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
	topic_change:
	this->pv_topic.clear();
	this->pv_topic = tp;
	broadcast_message("The new topic of this channel (" + this->getName() + ") is now " + tp + ".\n");
}

void channel::lift_topic_restriction(user *sender)
{
	if (sender->getAdmin())
	{
		this->pv_topic_op_restricted = false;
		return broadcast_message("The topic of this channel (" + this->getName() + ") can now be freely changed by everyone\n");
	}
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		if (*it == sender)
		{
			this->pv_topic_op_restricted = false;
			return broadcast_message("The topic of this channel (" + this->getName() + ") can now be freely changed by everyone\n");

		}
		it ++;
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
}

void channel::enforce_topic_restriction(user *sender)
{
	if (sender->getAdmin())
	{
		this->pv_topic_op_restricted = true;
		return broadcast_message("The topic of this channel (" + this->getName() + ") can now only be changed by a moderator or an administrator\n");
	}
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		if (*it == sender)
		{
			this->pv_topic_op_restricted = true;
			return broadcast_message("The topic of this channel (" + this->getName() + ") can now only be changed by a moderator or an administrator\n");
		}
		it ++;
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
}

void channel::lift_invite_restriction(user *sender)
{
	if (sender->getAdmin())
	{
		this->pv_invite_only = false; //faut clear la whitelist?
		return broadcast_message("This channel (" + this->getName() + ") is not in invite-only mode anymore\n");
	}
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		if (*it == sender)
		{
			this->pv_invite_only = false;//faut clear la whitelist?
			return broadcast_message("This channel (" + this->getName() + ") is not in invite-only mode anymore\n");
		}
		it ++;
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
}

void channel::enforce_invite_restriction(user *sender)
{
	if (sender->getAdmin())
	{
		this->pv_invite_only = true; //il faut donc mettre tous les users actuels dans la whitelist? nan vu qu'ils ont déjà join et sont déjà la?
		return broadcast_message("This channel (" + this->getName() + ") is now in invite-only mode\n");
	}
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		if (*it == sender)
		{
			this->pv_invite_only = true;
			return broadcast_message("This channel (" + this->getName() + ") is now in invite-only mode\n");
		}
		it ++;
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
}

void channel::lift_password_restriction(user *sender)
{
	if (sender->getAdmin())
	{
		this->pv_needs_password = false;
		this->pv_password = "";
		return broadcast_message("This channel (" + this->getName() + ") doesn't require a password anymore\n");
	}
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		if (*it == sender)
		{
			this->pv_needs_password = false;
			this->pv_password = "";
			return broadcast_message("This channel (" + this->getName() + ") doesn't require a password anymore\n");
		}
		it ++;
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
}

void channel::enforce_password_restriction(user *sender, const std::string &pw)
{
	if (sender->getAdmin())
	{
		if (pw.length() == 0)
			return (sender->recieve_message("Channel password can't be empty\n"));
		this->pv_needs_password = true;
		this->pv_password = pw;
		return broadcast_message("This channel (" + this->getName() + ") does now require a password to enter\n");
	}
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		if (*it == sender)
		{
			if (pw.length() == 0)
				return (sender->recieve_message("channel password can't be empty\n"));
			this->pv_needs_password = true;
			this->pv_password = pw;
			return broadcast_message("This channel (" + this->getName() + ") does now require a password to enter\n");
		}
		it ++;
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
}

std::string cpp_ssizet_to_string(ssize_t num)
{
	std::string ret;
	std::stringstream ss;
	ss << num;
	ss >> ret;
	return ret;
}

void channel::lift_user_limit(user *sender)
{
	if (sender->getAdmin())
	{
		this->pv_user_limit = -1;
		return broadcast_message("This channel (" + this->getName() + ") has now a limit of " + cpp_ssizet_to_string(MAX_USERS_IN_CHAT) + "users\n");
	}
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		if (*it == sender)
		{
			this->pv_user_limit = -1;
			return broadcast_message("This channel (" + this->getName() + ") has now a limit of " + cpp_ssizet_to_string(MAX_USERS_IN_CHAT) + "users\n");
		}
		it ++;
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
}

void channel::enforce_user_limit(user *sender, ssize_t limit)
{
	if (sender->getAdmin())
	{
		if (limit < 1)
			return (sender->recieve_message("User limit must be greater than zero\n"));
		this->pv_user_limit = limit;
		return broadcast_message("This channel (" + this->getName() + ") has now a limit of " + cpp_ssizet_to_string(limit) + "users\n");
	}
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		if (*it == sender)
		{
			if (limit < 1)
				return (sender->recieve_message("User limit must be greater than zero\n"));
			this->pv_user_limit = limit;
			return broadcast_message("This channel " + this->getName() + " has now a limit of " + cpp_ssizet_to_string(limit) + "users\n");
		}
		it ++;
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
}

void channel::promote_to_op(user *sender, std::string reciever)
{
	if (!(sender->getAdmin()))
		return (sender->recieve_message("Request denied. insufficent privileges\n"));

	user *test;
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		test = *it;
		if (test->getUserName() == reciever)
			return (sender->recieve_message(test->getNick() + " already is operator in said channel\n"));
		it ++;
	}
	this->pv_chann_modos.push_back(*it);
	test->recieve_message("You now are moderator in \"" + this->getName() + "\" channel.\n");
}

void channel::demote(user *sender, std::string reciever)
{
	if (!(sender->getAdmin()))
		return (sender->recieve_message("Request denied. insufficent privileges\n"));

	user *test;
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		test = *it;
		if (test->getUserName() == reciever)
		{
			this->pv_chann_modos.erase(it);
			return ;
		}
		it ++;
	}
	return (sender->recieve_message("target user is not an operator in said channel\n"));
	test->recieve_message("You were stripped from your moderator privileges in \"" + this->getName() + "\" channel.\n");
}

void channel::invite_user(user *sender, std::string reciever)
{
	user *test;
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	//pourquoi j'ai dû le foutre avant le goto alors que je l'utilise pas dedans, j'en sais rien
	if (sender->getAdmin())
		goto add_user;
	while (it != last)
	{
		if (*it == sender)
			goto add_user;
		it ++;
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
	add_user:
	std::vector<user *>::iterator ita = pv_whitelist.begin(), lasta = pv_whitelist.end();
	while (ita != lasta)
	{
		test = *ita;
		if (test->getUserName() == reciever)
			return (sender->recieve_message("User already has access privileges\n"));
		ita ++;
	}
	this->pv_whitelist.push_back(test);
	this->broadcast_message((std::string) reciever + " has been invited to this channel by " + sender->getNick() + ".\n");
	test->recieve_message("You were invited to join \"" + this->getName() + "\" channel.\n");
}

//kick : 	admins can't be kicked;
//			an operator can't kick another operator;
//			si sender n'est pas admin ni operator, error;
//			si reciever n'est pas dans le chann, error;
//			si reciever est opérator il faut le remove de la liste;
//			dans tous les cas il faut le virer des users du chann
void channel::kick_user(user *sender, std::string reciever)
{
	std::vector<user *>::iterator ituser = pv_users_in_chann.begin(), lastuser = pv_users_in_chann.end();
	user *testuser, *testrec;
	while (ituser != lastuser)
	{
		testuser = *ituser;
		if (testuser->getUserName() == reciever)
			break;
		ituser ++;
	}
	if (ituser == lastuser)
		return (sender->recieve_message("Target user isn't present in this channel\n"));
	if (testuser->getAdmin())
		return (sender->recieve_message("Target is an administrator and cannot be kicked\n"));
	std::vector<user *>::iterator itsend = pv_chann_modos.begin(), itrec = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (itsend != last)
	{
		if (sender->getAdmin())
			break ;
		if (*itsend == sender)
			break ;
		itsend ++;
	}
	while (itrec != last)
	{
		testrec = *itrec;
		if (testrec->getUserName() == reciever)
			break ;
		itrec ++;
	}
	if (sender->getAdmin())
	{
		if (itrec != pv_chann_modos.end())
			pv_chann_modos.erase(itrec);
		pv_users_in_chann.erase(ituser);
		testrec->recieve_message("You have been kick from \"" + this->getName() + "\" channel by " + sender->getNick() + "\n");
		return (broadcast_message(sender->getNick() + " kicked " + testrec->getNick() + "\n"));
	}
	if (itsend != pv_chann_modos.end())
	{
		if (itrec != pv_chann_modos.end())
			return (sender->recieve_message("You can't kick target user, he has same privileges than you\n"));
		pv_users_in_chann.erase(ituser);
		testrec->recieve_message("You have been kick from \"" + this->getName() + "\" channel by " + sender->getNick() + "\n");
		return (broadcast_message(sender->getNick() + " kicked " + testrec->getNick() ));
	}
	return (sender->recieve_message("Request denied. insufficent privileges\n"));
}

std::string channel::show_users()
{
	std::string users = "The users in this channel (" + this->getName() + ") are : ";
	std::vector<user *>::iterator it = pv_users_in_chann.begin(), last = pv_users_in_chann.end();
	while (it != last)
	{
		user *u = *it;
		users.append(u->getNick() + " ;");
		it ++;
	}
	users.append(" .\n");
	return (users);
}

std::string channel::show_mods() 
{
	std::string mods = "The moderators in this channel ( " + this->getName() + " ) are : ";
	std::vector<user *>::iterator it = pv_chann_modos.begin(), last = pv_chann_modos.end();
	while (it != last)
	{
		user *u = *it;
		mods.append(u->getNick() + " ;");
		it ++;
	}
	mods.append(" .\n");
	return (mods);
}

size_t channel::count_users() const
{
	return ((size_t)pv_users_in_chann.size());
}

bool channel::getInviteStatus() const
{
	return (pv_invite_only);
}

bool channel::getPasswordStatus() const
{
	return (pv_needs_password);
}

std::string channel::getPassword() const
{
	return (pv_password);
}

std::string channel::getName() const
{
	return (pv_name);
}