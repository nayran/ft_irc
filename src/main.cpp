#include "ft_irc.hpp"
#include "serverClass.hpp"

int main(int argc, char **argv)
{
	try
	{
		if (argc != 3)
			throw std::runtime_error("usage: ./ircserv <port> <password>");
		Server server(HOST, argv[1], argv[2]);
		server.start();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return (1);
	}
	return (0);
}
