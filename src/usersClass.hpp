#ifndef USERSCLASS_HPP
# define USERSCLASS_HPP

#include "ft_irc.hpp"
#include <list>

class User
{
	public:
		User(int client);
		~User();

	private:
		int					_clientfd;
};

#endif
