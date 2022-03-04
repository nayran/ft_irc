#include "ft_irc.hpp"

int _err (std::string s)
{
	std::cout << s << std::endl;
	return (1);
}

int main(int argc, char **argv)
{
	int socketfd;
	if (argc != 3)
		return (_err("server port and password expected!"));
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
		return (_err("error creating socket!"));
	std::cout << "socketfd: " << socketfd << "\nserver port: " << argv[1] << std::endl;
	return (0);
}
