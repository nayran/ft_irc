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

void ft_flush(std::string s)
{
	std::cout << "flush:" << s << "-" << std::endl
			  << std::flush;
}

void Command::parse(std::string buff)
{
	buff.erase(std::remove(buff.begin(), buff.end(), '\n'), buff.end());
	buff.erase(std::remove(buff.begin(), buff.end(), '\r'), buff.end());

	size_t pos = 0;
	while ((pos = buff.find(' ')) != std::string::npos)
	{
		_options.push_back(buff.substr(0, pos));
		buff.erase(0, pos + 1);
	}
	_options.push_back(buff.substr(0, buff.size()));
	_command = *_options.begin();
	_options.erase(_options.begin());

	if ((*_options.begin())[0] == ':')
	{
		_options.begin()->erase(0, 1);
		if (*_options.begin() == "")
			_options.erase(_options.begin());
	}
}

void Command::run()
{
	if (_command == "CAP")
		_user.userResponse("CAP * ACK multi-prefix");
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
			else if (_command == "OPER")
				ft_oper();
			else if (_command == "JOIN")
				ft_join();
			else
			{
				std::string ack = "Unknown command: " + _command;
				std::cout << ack << std::endl;
				_user.userResponse(ack);
				std::cout << "new: " << _command;
				for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
					std::cout << " " << *it;
				std::cout << std::endl;
			}
			// else if (_command == "QUIT")
			// {
			// 	ft_quit();
			// }
		}
		else
			ft_userexcept();
	}
	else
		ft_passexcept();
}

void Command::ft_join()
{
	if (*_options.begin() == "#")
	{
		// _options.begin()->erase(0, 1);
		// if (*_options.begin() == "")
		_options.erase(_options.begin());
	}
	if (_options.size() < 1 || _options.size() > 2)
		return _user.userResponse("usage: /JOIN <channel> [<channel_password>]");
	Channel *channel = _server.getChannelByName(_options[0]);
	if (_options.size() == 1)
		_options.push_back("");
	if (!channel)
	{
		channel = new Channel(_options[0], _options[1]);
		_server.addChannel(channel);
	}
	if (_options[1] != channel->getPassword())
		return _user.userResponse("Wrong channel password");
	else
		_user.addChannel(channel);
	// std::string ack;
	// ack = " JOIN " + channel->getName();
	// ack = _user.getNick() + " joined the channel " + _options[0];
	// ack = "JOIN " + channel->getName() + " :" + "joinedchannel";
	channel->channelResponse("welcome to the channel");
}

void Command::ft_pass()
{
	if (_options.size() != 1)
		return _user.userResponse("usage: /PASS <password>");
	if (_user.isAuth())
		return _user.userResponse("You are already registered");
	if (_options[0] == _server.getPassword())
		_user.auth();
	else
		return _user.userResponse("Wrong password");
	std::string ack = "Welcome to Nayran's ft_irc " + _user.getNick();
	_user.userResponse(ack);
}

void Command::ft_nick()
{
	std::string newNick = _options[0];
	if (_options.size() != 1 && !_user.getNick().empty())
		return _user.userResponse("usage: /NICK <newNick>");
	if (newNick.length() < 3)
		return _user.userResponse("newNick must have at least 3 chars!");
	int i = -1;
	while (newNick.c_str()[++i + 1])
	{
		if (!isalnum(newNick.c_str()[i]))
			return _user.userResponse("newNick cannot have special chars!");
	}
	std::list<User *> users = _server.getUsers();
	std::list<User *>::iterator it = users.begin();
	for (; it != users.end(); ++it)
	{
		if ((*it)->getNick() == newNick)
			return (*it)->userResponse("Nick already in use!");
	}
	if (_user.getNick() != "")
		_server.serverResponse(_user.getNick() + " changed nick to " + newNick);
	else
		_server.serverResponse(newNick + " joined the server!");
	_user.setNick(*_options.begin());
}

void Command::ft_user()
{
	if (_options.size() != 4)
		return _user.userResponse("usage: /USER <username> <hostname> <servername> <realname>");
	if (!_user.getUsername().empty())
		return _user.userResponse("USER is already registered");
	_user.setUsername(_options[0]);
	_user.setRealname(_options[3]);
	// std::string ack = "USER " + _options[0] + " 0 * " + _options[3];
	// userResponse(ack);
	// send(_user.getSocket(), ack.c_str(), strlen(ack.c_str()), 0);
}

void Command::ft_quit()
{
	std::string ack = "Quit";
	if (_options.size() > 0)
	{
		ack += ": ";
		for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
			ack += *it + " ";
	}
	std::cout << ack << std::endl;
	// _server.deleteUser(&this->_user);
	// _server.serverResponse(ack);
}

void Command::ft_oper()
{
	if (_options.size() != 2)
		return _user.userResponse("usage: /OPER <nickname> <oper_password>");
	User *u = _server.getUserByNick(_options[0]);
	if (u == nullptr)
		return _user.userResponse("There's no user with this nick!");
	if (u->isOper())
		return _user.userResponse(_options[0] + " is an operator already!");
	if (_options[1] == OPER_PASS)
		u->setOper();
	else
		return _user.userResponse("Wrong password");
	_user.userResponse(_options[0] + " became an operator!");
	u->userResponse("You became an operator. With great power comes great responsibility!");
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
	_user.userResponse(ack);
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
	_user.userResponse(ack);
}