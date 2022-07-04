#include "usersClass.hpp"

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