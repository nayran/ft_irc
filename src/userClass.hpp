#ifndef USERSCLASS_HPP
#define USERSCLASS_HPP

#include "ft_irc.hpp"
#include <cctype>

class Server;
class Channel;

class User
{
public:
	User(int client);
	~User();
	int getSocket();

	std::string getNick();
	std::string getUsername();
	// std::string getRealname();
	void setNick(std::string);
	void setUsername(std::string username);
	void setRealname(std::string username);
	bool isAuth();
	void auth();
	bool isOper();
	void setOper();
	void addChannel(Channel *channel);
	void messageUser(std::string message);
	void deleteChannel(Channel *channel);

private:
	int _clientfd;
	std::string _nick;
	std::string _username;
	std::string _realname;
	bool _auth;
	bool _oper;
	std::list<Channel *> _channels;
};

#include "channelClass.hpp"
#include "serverClass.hpp"

#endif
