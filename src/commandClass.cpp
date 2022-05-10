#include "commandClass.hpp"

Command::Command(std::string buff, int clisocket, Server &server) : 
	_server(server), _user(*server.getUserBySocket(clisocket))
{
	parse(buff);
	run();
	
	//std::vector<std::string>::iterator it;
	//for(it = _command.begin(); it != _command.end(); it++)
	//	std::cout << *it << std::endl;

	
};

Command::~Command()
{};

void	Command::parse(std::string buff)
{
	buff.erase(std::remove(buff.begin(), buff.end(), '\n'), buff.end());
	std::stringstream iss(buff);
	std::string val;

	while (std::getline(iss, val, ' '))
		_options.push_back(val);
	_command = *_options.begin();
	_options.erase(_options.begin());

	std::cout << _command ;
	for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
		std::cout << " " << *it;
	std::cout << std::endl;
}

void	Command::run()
{
	if (_command == "PASS")
		std::cout << _server.getPassword() << std::endl;
	else if (_command == "NICK")
		ft_nick();
	else if (_command == "USER")
		ft_user();
	else if (!_user.getNick().empty())
	{
		std::cout << _command ;
		for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
			std::cout << " " << *it;
		std::cout << std::endl;
	}
	else
		ft_exceptions();
}

void	Command::ft_nick()
{
	if (_options.size() != 1 && !_user.getNick().empty())
	{
		std::cout << "usage: /NICK <newNick>" << std::endl;
		return ;
	}
	_user.setNick(*_options.begin());
}

void	Command::ft_user()
{
	if (_options.size() < 4)
	{
		std::cout << "usage: /USER <username> <hostname> <servername> <realname>" << std::endl;
		return ;
	}
	_user.setUsername(_options[0]);
	_user.setRealname(_options[3]);
	std::string ack = ": USER " + _options[0] + " 0 * " + _options[3] + "\r\n";
	send(_user.getSocket(), ack.c_str(), strlen(ack.c_str()), 0);
	std::cout << ack << std::endl;
}

void	Command::ft_exceptions()
{
	std::cout << _command ;
	for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
		std::cout << " " << *it;
	std::cout << std::endl;
	std::cout << "Please, provide a nick to execute commands" << std::endl;
	send(_user.getSocket(), "test", 5, 0);
}
