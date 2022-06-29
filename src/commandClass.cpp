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
	buff.erase(std::remove(buff.begin(), buff.end(), '\r'), buff.end());

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
	if (*_options.begin() == ":")
		_options.erase(_options.begin());
	else if ((*_options.begin())[0] == ':')
		_options.begin()->erase(0, 1);
}

void Command::run()
{
	if (_command == "CAP")
		response(&_user, "CAP * ACK multi-prefix");
	else if (_command == "PASS")
		ft_pass();
	else if (_user.isAuth())
	{
		if (_command == "NICK")
			ft_nick();
		else if (!_user.getNick().empty())
		{
			if (_command == "USER")
				ft_user();
			else if (_command == "QUIT")
				ft_quit();
			else
			{
				std::string ack = "Unknown command: " + _command;
				std::cout << ack << std::endl;
				response(&_user, ack);
				// std::cout << "new: " << _command;
				// for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
				// 	std::cout << " " << *it;
				// std::cout << std::endl;
			}
		}
		else
			ft_userexcept();
	}
	else
		ft_passexcept();
	// _server.sendMessage("teste");
}

void Command::ft_pass()
{
	if (_options.size() != 1)
		return response(&_user, "usage: /PASS <password>");
	if (_user.isAuth())
		return response(&_user, "You are already registered");
	if (_options[0] == _server.getPassword())
		_user.auth();
	else
		return response(&_user, "Wrong password");
	std::string ack = "Welcome to Nayran's ft_irc " + _user.getNick();
	response(&_user, ack);
}

void Command::ft_nick()
{
	if (_options.size() != 1 && !_user.getNick().empty())
		return response(&_user, "usage: /NICK <newNick>");
	_user.setNick(*_options.begin());
}

void Command::ft_user()
{
	if (_options.size() != 4)
		return response(&_user, "usage: /USER <username> <hostname> <servername> <realname>");
	if (!_user.getUsername().empty())
		return response(&_user, "USER is already registered");
	_user.setUsername(_options[0]);
	_user.setRealname(_options[3]);
	std::string ack = "USER " + _options[0] + " 0 * " + _options[3];
	response(&_user, ack);
	// send(_user.getSocket(), ack.c_str(), strlen(ack.c_str()), 0);
}

void Command::ft_quit()
{
	std::string ack = "Quit";
	if (_options.size() > 0)
		ack += ": ";
	for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
		ack += *it + " ";
	std::cout << ack << std::endl;
	response(&_user, ack);
	_server.deleteUser(&this->_user);
}

void Command::ft_userexcept()
{
	// std::cout << "USER EXCEPTIONS" << std::endl;
	// std::cout << _command;
	// for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
	// 	std::cout << " " << *it;
	// std::cout << std::endl;
	std::string ack = "Please, provide a nick to execute commands - usage: /NICK <newNick>";
	std::cout << ack << std::endl;
	response(&_user, ack);
}

void Command::ft_passexcept()
{
	// std::cout << "PASS EXCEPTIONS" << std::endl;
	// std::cout << _command;
	// for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
	// 	std::cout << " " << *it;
	// std::cout << std::endl;
	std::string ack = "Please, provide a password to execute commands - usage: /PASS <password>";
	std::cout << ack << std::endl;
	response(&_user, ack);
}

void response(User *user, std::string ack)
{
	std::string res;
	if (!user->getNick().empty())
		res = user->getNum() + " " + user->getNick() + " :" + ack + "\r\n";
	else
		res = user->getNum() + " nick :" + ack + "\r\n";
	// std::cout << res << std::endl;
	send(user->getSocket(), res.c_str(), strlen(res.c_str()), 0);
}