#include "commandClass.hpp"

Command::Command(std::string buff)
{
	static int x;
	//std::cout << buff;
	std::cout << x++ << buff << std::endl;

	

};

Command::~Command()
{};
