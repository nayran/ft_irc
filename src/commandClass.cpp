#include "commandClass.hpp"

Command::Command(std::string buff, int clisocket, Server &server) : _server(server), _user(*server.getUserBySocket(clisocket))
{
	parse(buff);
	run();

	// std::vector<std::string>::iterator it;
	// for(it = _command.begin(); it != _command.end(); it++)
	//	std::cout << *it << std::endl;
};

Command::~Command(){};

void Command::parse(std::string buff)
{
	buff.erase(std::remove(buff.begin(), buff.end(), '\n'), buff.end());

	// std::stringstream iss(buff);
	// std::string val;

	size_t pos = 0;

	while ((pos = buff.find(' ')) != std::string::npos)
	{
		_options.push_back(buff.substr(0, pos));
		buff.erase(0, pos + 1);
	}
	_options.push_back(buff.substr(0, buff.size()));
	_command = *_options.begin();
	_options.erase(_options.begin());

	// std::cout << "Command:" << _command ;
	// for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
	// 	std::cout << " " << *it;
	// std::cout << std::endl;
}

void Command::run()
{
	if (_command == "CAP")
		response(&_user, "CAP * ACK multi-prefix");
	else if (_command == "PASS")
		std::cout << _server.getPassword() << std::endl;
	else if (_command == "NICK")
		ft_nick();
	else if (_command == "USER")
		ft_user();
	else if (_command == "QUIT")
		ft_quit();
	else if (!_user.getNick().empty())
	{
		std::cout << "new: " << _command;
		for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
			std::cout << " " << *it;
		std::cout << std::endl;
	}
	else
		ft_exceptions();
}

void Command::ft_nick()
{
	if (_options.size() != 1 && !_user.getNick().empty())
	{
		response(&_user, "usage: /NICK <newNick>");
		return;
	}
	_user.setNick(*_options.begin());
}

void Command::ft_user()
{
	if (_options.size() < 4 || _options.size() > 4)
	{
		response(&_user, "usage: /USER <username> <hostname> <servername> <realname>");
		return;
	}
	if (!_user.getUsername().empty())
	{
		response(&_user, "USER is already registered");
		return;
	}
	_user.setUsername(_options[0]);
	// if (_options[3][0] == ':')
	// _options[3].erase(0, 1);
	_user.setRealname(_options[3]);
	std::string ack = "USER " + _options[0] + " 0 * " + _options[3];
	response(&_user, ack);
	send(_user.getSocket(), ack.c_str(), strlen(ack.c_str()), 0);
}

void Command::ft_quit()
{
	std::string ack = "Quit: ";
	for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
		ack += *it;
	response(&_user, ack);
	// response(&_user, "Server shutting down...");
	// exit(0);
}

void Command::ft_exceptions()
{
	std::cout << "EXCEPTIONS" << std::endl;
	std::cout << _command;
	for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
		std::cout << " " << *it;
	std::cout << std::endl;
	std::cout << "Please, provide a nick to execute commands" << std::endl;
	send(_user.getSocket(), "test", 5, 0);
}

void response(User *user, std::string ack)
{
	std::string res = user->getNum() + " " + user->getNick() + " :" + ack + "\r\n";
	// std::cout << res << std::endl;
	send(user->getSocket(), res.c_str(), strlen(res.c_str()), 0);
}