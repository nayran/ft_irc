#ifndef USERSCLASS_HPP
#define USERSCLASS_HPP

#include "ft_irc.hpp"
#include <cctype>

class Server;

class User
{
public:
	User(int client);
	~User();
	int getSocket();

	std::string getNick();
	std::string getUsername();
	std::string getNum();
	// std::string getRealname();
	void setNick(std::string);
	void setUsername(std::string username);
	void setRealname(std::string username);

private:
	int _clientfd;
	std::string _nick;
	std::string _username;
	std::string _realname;
	int _num;
};

#include "serverClass.hpp"

#endif
