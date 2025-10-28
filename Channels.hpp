#ifndef CHANNELS_HPP
# define CHANNELS_HPP

# include <iostream>
# include "Users.hpp"
# include "Server.hpp"
# include <vector>
# include <sstream>

class user;
class server;

class channel{
    	bool				pv_invite_only;
    	bool				pv_topic_op_restricted;
    	bool				pv_needs_password;
		std::string			pv_name;
		std::string			pv_password;
		std::string			pv_topic;
    	ssize_t 			pv_user_limit;
		std::vector<user *>	pv_users_in_chann;//ca ou un vector d'ints pour les ID? pointeur sur user semble plus simple tho
		std::vector<user *> pv_whitelist;
		std::vector<user *>	pv_chann_modos;
		server				*serv;
	public :
		channel(std::string &name);
		~channel();
		
		void close();

		void broadcast_message(const std::string &msg);
		void broadcast_message(std::string msg, user *sender);
		void warn_user(std::string &msg, user *sender);
		void analyse_msg_content(std::string &msg, user *send);

		void change_topic(user *sender, std::string &tp);
		void lift_topic_restriction(user *sender);
		void enforce_topic_restriction(user *sender);
		void lift_invite_restriction(user *sender);
		void enforce_invite_restriction(user *sender);
		void lift_password_restriction(user *sender);
		void enforce_password_restriction(user *sender, const std::string &pw);
		void lift_user_limit(user *sender);
		void enforce_user_limit(user *sender, ssize_t limit);
		void kick_user(user *sender, std::string reciever);
		void promote_to_op(user *sender, std::string reciever);
		void demote(user *sender, std::string reciever);
		void invite_user(user *sender, std::string reciever);

		void kick(std::string &, user *);
		void invite(std::string &, user *);
		void topic(std::string &, user *);
		void mode(std::string &, user *);
		void join(std::string &, user *);
		void leave(std::string &, user *);
		void display(std::string &, user *);
		void parse_string(std::string &, user *);

		std::string show_mods();
		std::string show_users();
		void check_and_send(user *send);
		size_t count_users() const;
		void display_active_chann_users(user *);

		bool getInviteStatus() const;
		bool getPasswordStatus() const;
		std::string getPassword() const;
		std::string getName() const;
};

std::string cpp_ssizet_to_string(ssize_t num);
ssize_t cpp_string_to_ssizet(std::string &s);
#endif