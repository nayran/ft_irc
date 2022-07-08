#include "ft_irc.hpp"
#include "serverClass.hpp"
#include <csignal>

int running = true;

void shutdown(int sig)
{
	std::cout << "Server shutting down..." << sig << std::endl;
	running = false;
}

int main(int argc, char **argv)
{
	signal(SIGINT, shutdown);
	try
	{
		if (argc != 3)
			throw std::runtime_error("usage: ./ircserv <port> <password>");
		Server server(HOST, argv[1], argv[2]);
		server.init();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return (1);
	}
	return (0);
}
