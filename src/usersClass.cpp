#include "usersClass.hpp"

User::User(int client)
{
	this->_clientfd = client;
	this->nick = "";
};

User::~User()
{};

void	User::setNick(std::string newNick)
{ 
	std::cout << "Before: " << this->nick << std::endl;
	this->nick = newNick;
}

int		User::getSocket()
{
	return (this->_clientfd);
}

std::string User::getNick()
{
	return (this->nick);
}
