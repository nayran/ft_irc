#include "commandClass.hpp"

Command::Command(std::string buff, int clisocket, Server *server)
{
	parse(buff);
	_user = server->getUserBySocket(clisocket);
	run();
	
	//std::vector<std::string>::iterator it;
	//for(it = _command.begin(); it != _command.end(); it++)
	//	std::cout << *it << std::endl;

	
};

Command::~Command()
{};

void	Command::parse(std::string buff)
{
	buff.erase(remove(buff.begin(), buff.end(), '\n'), buff.end());
	std::stringstream iss(buff);
	std::string val;

	while (std::getline(iss, val, ' '))
		_commands.push_back(val);
	_command = *_commands.begin();
	_commands.erase(_commands.begin());
}

void	Command::run()
{
	if (_command == "NICK")
		_user->setNick(*_commands.begin());
	else
		std::cout << "after: " << _user->getNick() << std::endl;
}
