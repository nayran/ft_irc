#include "ft_irc.hpp"

int _err (std::string s)
{
	std::cout << s << std::endl;
	return (0);
}

int main(int argc, char **argv)
{
	if (argc != 3)
		return (_err("server port and password expected!"));
	std::cout << "server port: " << argv[1] << std::endl;
	return (0);
}
