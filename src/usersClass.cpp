#include "usersClass.hpp"

User::User(int client)
{
	this->_clientfd = client;
	this->_nick = "";
};

User::~User()
{};

void	User::setNick(std::string newNick)
{ 
	int i = -1;
	if (newNick.length() < 4)
		response(this, "newNick must has 3 chars!");
	else
	{
		while (newNick.c_str()[++i + 1])
		{
			if (!isalnum(newNick.c_str()[i]))
			{
				response(this, "newNick cannot have special chars!");
				return ;
			}
		}
		this->_nick = newNick;
		//std::string ack = "new Nick: " + newNick;
		static int num;
		num++;
		std::string ack = "00" + std::to_string(num) + " " + _nick + " :Welcome to Nayran's ft_irc " + _nick;
		response(this, ack);
	}
}

int		User::getSocket()
{
	return (this->_clientfd);
}

std::string User::getNick()
{
	return (this->_nick);
}
		
void	User::setUsername(std::string username)
{
	_username = username;
}

std::string User::getUsername()
{
	return (this->_username);
}

void	User::setRealname(std::string realname)
{
	_realname = realname;
}

std::string User::getRealname()
{
	return (this->_realname);
}
