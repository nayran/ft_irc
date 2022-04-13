#include "commandClass.hpp"

Command::Command(char *buff)
{
	std::cout << buff << std::flush;
	memset(buff, 0, sizeof(&buff));

};

Command::~Command()
{};
