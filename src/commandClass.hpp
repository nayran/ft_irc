#ifndef COMMANDCLASS_HPP
# define COMMANDCLASS_HPP

#include "ft_irc.hpp"
#include <sstream>
#include <algorithm>

class Server;
class User;

class Command 
{
	public:
		Command(std::string buff, int client, Server &server);
		~Command();

		void	ft_nick();
		void	ft_user();
		void	ft_exceptions();

	private:
		std::vector<std::string>	_options;
		std::string					_command;
		Server						&_server;
		User						&_user;

		void						parse(std::string buff);
		void						run();
};

#include "serverClass.hpp"
#include "usersClass.hpp"

#endif
