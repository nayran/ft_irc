#include "commandClass.hpp"

Command::Command(std::string buff, int clisocket, Server &server) : 
	_server(server), _user(*server.getUserBySocket(clisocket))
{
	parse(buff);
	//run();
	
	//std::vector<std::string>::iterator it;
	//for(it = _command.begin(); it != _command.end(); it++)
	//	std::cout << *it << std::endl;

	
};

Command::~Command()
{};

void	Command::parse(std::string buff)
{
	//buff.erase(remove(buff.begin(), buff.end(), '\n'), buff.end());
	std::stringstream iss(buff);
	std::string val;

	while (std::getline(iss, val, ' '))
		_options.push_back(val);
	_command = *_options.begin();
	_options.erase(_options.begin());
	std::cout << "C:" << _command;
	for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
		std::cout << " " << *it;
	std::cout << std::endl;
}

void	Command::run()
{
	std::cout << _command << std::endl << std::endl;
	if (_command == "PASS")
		std::cout << _server.getPassword() << std::endl;
	else if (_command == "NICK")
	{
		if (_options.size() != 1 && !_user.getNick().empty())
			std::cout << _options.size() << "usage: /nick <newNick>" << std::endl;
		else
			_user.setNick(*_options.begin());
	}
	else if (_command == "USER")
	{
		std::cout << _command ;
		for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
			std::cout << " " << *it;
		std::cout << std::endl;
	}
	else if (!_user.getNick().empty())
	{
		std::cout << _command ;
		for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
			std::cout << " " << *it;
		std::cout << std::endl;
	}
	else
	{
		std::cout << _command ;
		std::cout << std::endl;
		for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
			std::cout << " " << *it;
		std::cout << std::endl;
		std::cout << "Please, provide a nick to execute commands" << std::endl;
		send(_user.getSocket(), "test", 5, 0);
	}
}
