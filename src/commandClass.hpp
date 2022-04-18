#ifndef COMMANDCLASS_HPP
# define COMMANDCLASS_HPP

#include "ft_irc.hpp"
#include <sstream>

class Server;
class User;

class Command 
{
	public:
		Command(std::string buff, int client, Server *server);
		~Command();

	private:
		std::vector<std::string>	_commands;
		std::string					_command;
		User						*_user;

		void						parse(std::string buff);
		void						run();
};

#include "serverClass.hpp"
#include "usersClass.hpp"

#endif