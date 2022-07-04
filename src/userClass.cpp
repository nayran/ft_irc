#include "userClass.hpp"

User::User(int client)
{
	this->_clientfd = client;
	this->_nick = "";
	static int num;
	this->_num = ++num;
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
			return this->userResponse("You already joined this channel!");
	}
	_channels.push_back(channel);
	channel->addUser(this);
}

void User::userResponse(std::string ack)
{
	std::string res = ":127.0.0.1 " + this->getNum() + " ";
	res += this->getNick() + " :" + ack + "\r\n";
	// std::cout << res << std::endl;
	send(this->getSocket(), res.c_str(), strlen(res.c_str()), 0);
}