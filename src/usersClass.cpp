#include "usersClass.hpp"

User::User(int client)
	: _clientfd(client)
{
	std::cout << "Client: " << _clientfd << ":";
};

User::~User()
{};
