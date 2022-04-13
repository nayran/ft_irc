#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

#include "ft_irc.hpp"
#include "usersClass.hpp"
#include "commandClass.hpp"
#include <netdb.h>
#include <vector>
#include <exception>
#include <unistd.h>
#include <poll.h>

class Server
{
	public:
		Server(std::string host, std::string port, std::string password);
		~Server();
		void			setSockets();
		void			init();

	private:
		std::string		_host;
		std::string		_port;
		std::string		_password;
		int				_socket;
		std::vector<pollfd>	_fdvec;
		std::list<User *>	_users;
};

#endif
