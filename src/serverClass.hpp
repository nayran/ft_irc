#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

#include "ft_irc.hpp"
#include <netdb.h>
#include <exception>
#include <unistd.h>
#include <poll.h>

class User;
class Command;

class Server
{
	public:
		Server(std::string host, std::string port, std::string password);
		~Server();
		void			setSockets();
		void			init();
		User*			getUserBySocket(int socket);
		//int				getSocket();
		std::string		getPassword();

	private:
		std::string		_host;
		std::string		_port;
		std::string		_password;
		int				_socket;
		std::vector<pollfd>	_fdvec;
		std::list<User *>	_users;
};

#include "usersClass.hpp"
#include "commandClass.hpp"

#endif
