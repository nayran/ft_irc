#ifndef USERSCLASS_HPP
# define USERSCLASS_HPP

#include "ft_irc.hpp"
#include <cctype>

class Server;

class User
{
	public:
		User(int client);
		~User();
		int		getSocket();

		std::string getNick();
		void				setNick(std::string);
	private:
		int					_clientfd;
		std::string			_nick;

};

#include "serverClass.hpp"

#endif
