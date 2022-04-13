#include "usersClass.hpp"

User::User(int client)
{
	this->_clientfd = client;
};

User::~User()
{};
