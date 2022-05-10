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
	{
		std::cout << "Nick must has 3 chars!" << std::endl;
		//send(_user->getSocket(), "Nick must at least 3 chars!", 27, 0);
	}
	else
	{
		while (newNick.c_str()[++i + 1])
		{
			if (!isalnum(newNick.c_str()[i]))
			{
				std::cout << "Nick cannot have special chars!" << std::endl;
				return ;
			}
		}
		this->_nick = newNick;
		std::cout << "New nick: " << _nick << std::endl;
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
