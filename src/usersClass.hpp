#ifndef USERSCLASS_HPP
# define USERSCLASS_HPP

#include "ft_irc.hpp"

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
		std::string			nick;

};

#include "serverClass.hpp"

#endif
