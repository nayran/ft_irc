#include "usersClass.hpp"

User::User(int client)
{
	this->_clientfd = client;
	this->_nick = "";
	this->_num = 0;
};

User::~User(){};

void User::setNick(std::string newNick)
{
	std::string ack;
	int i = -1;
	if (newNick.length() < 4)
		return response(this, "newNick must has 3 chars!");
	while (newNick.c_str()[++i + 1])
	{
		if (!isalnum(newNick.c_str()[i]))
			return response(this, "newNick cannot have special chars!");
	}
	if (_nick != "")
		ack = _nick + " changed nick to " + newNick;
	else
		ack = "Welcome to Nayran's ft_irc " + newNick;
	this->_nick = newNick;
	static int num;
	if (!this->_num)
		_num = ++num;
	response(this, ack);
}

int User::getSocket()
{
	return (this->_clientfd);
}

std::string User::getNick()
{
	return (this->_nick);
}

void User::setUsername(std::string username)
{
	_username = username;
}

std::string User::getUsername()
{
	return (this->_username);
}

void User::setRealname(std::string realname)
{
	_realname = realname;
}

std::string User::getNum()
{
	std::string num = "00";
	num += std::to_string(this->_num);
	return (num);
}

// std::string User::getRealname()
// {
// 	return (this->_realname);
// }
