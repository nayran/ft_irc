#include "userClass.hpp"

User::User(int client)
{
	this->_clientfd = client;
	this->_nick = "";
	this->_auth = false;
	this->_oper = false;
};

User::~User(){};

void User::setNick(std::string newNick)
{
	this->_nick = newNick;
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

std::string User::getRealname()
{
	return (this->_realname);
}

bool User::isAuth()
{
	return (this->_auth);
}

void User::auth()
{
	this->_auth = true;
}

bool User::isOper()
{
	return (this->_oper);
}

void User::setOper()
{
	this->_oper = true;
}

void User::addChannel(Channel *channel)
{
	std::list<Channel *>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++)
	{
		if ((*it)->getName() == channel->getName())
			return;
	}
	_channels.push_back(channel);
	channel->addUser(this);
}

void User::deleteChannel(Channel *channel)
{
	std::list<Channel *>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++)
	{
		if ((*it) == channel)
		{
			_channels.erase(it);
			return;
		}
	}
}

void User::messageUser(std::string message)
{
	if (message.find("\r\n"))
		message += "\r\n";
	if (send(this->getSocket(), message.c_str(), strlen(message.c_str()), 0) == -1)
		throw std::runtime_error("Could't send messageUser");
}