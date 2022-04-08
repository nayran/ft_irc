#include "usersClass.hpp"

User::User(int client)
	: _clientfd(client)
{
	std::string str("PASS NICK USER\n\r");
	if (send(_clientfd, str.c_str(), str.length(), 0) == -1)
		throw std::runtime_error("error send");
	std::cout << "Client: " << _clientfd << ":" << std::flush;
};

User::~User()
{};
