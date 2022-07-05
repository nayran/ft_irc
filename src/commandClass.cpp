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
		numericResponse("CAP * ACK multi-prefix", 0, "");
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
			// 		// else if (_command == "JOIN")
			// 		// ft_join();
			else
			{
				std::string ack = "Unknown command: " + _command;
				std::cout << ack << std::endl;
				numericResponse(ack, 0, "001");
				std::cout << "new: " << _command;
				for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
					std::cout << " " << *it;
				std::cout << std::endl;
			}
			// 		// else if (_command == "QUIT")
			// 		// {
			// 		// 	ft_quit();
			// 		// }
		}
		else
			ft_exception("nick");
	}
	else
		ft_exception("pass");
}

// void Command::ft_join()
// {
// 	if (*_options.begin() == "#")
// 	{
// 		// _options.begin()->erase(0, 1);
// 		// if (*_options.begin() == "")
// 		_options.erase(_options.begin());
// 	}
// 	if (_options.size() < 1 || _options.size() > 2)
// 		return _user.userResponse("usage: /JOIN <channel> [<channel_password>]");
// 	Channel *channel = _server.getChannelByName(_options[0]);
// 	if (_options.size() == 1)
// 		_options.push_back("");
// 	if (!channel)
// 	{
// 		channel = new Channel(_options[0], _options[1]);
// 		_server.addChannel(channel);
// 	}
// 	if (_options[1] != channel->getPassword())
// 		return _user.userResponse("Wrong channel password");
// 	else
// 		_user.addChannel(channel);
// 	// std::string ack;
// 	// ack = " JOIN " + channel->getName();
// 	// ack = _user.getNick() + " joined the channel " + _options[0];
// 	// ack = "JOIN " + channel->getName() + " :" + "joinedchannel";
// 	channel->channelResponse("welcome to the channel");
// }

void Command::ft_pass()
{
	if (_options.size() != 1 || _options[0] == "")
		return numericResponse("usage: /PASS <password>", 0, "461");
	if (_user.isAuth())
		return numericResponse("You are already registered", 0, "462");
	if (_options[0] == _server.getPassword())
		_user.auth();
	else
		return numericResponse("Wrong password", 0, "461");
	numericResponse("Welcome to Nayran's ft_irc " + _user.getNick(), 0, "001");
}

void Command::ft_nick()
{
	std::string newNick = _options[0];
	if (_options.size() != 1)
		return numericResponse("usage: /NICK <newNick>", 0, "431");
	if (newNick.length() < 3)
		return numericResponse("newNick must have at least 3 chars!", 0, "432");
	if (newNick == "anonymous" || newNick == "unknown")
		return numericResponse("This nick cannot be used!", 0, "432");
	for (size_t i = 0; i < newNick.length(); i++)
	{
		std::cout << newNick + "-" << std::endl;

		if ((newNick[i] >= 48 && newNick[i] <= 57) ||
			(newNick[i] >= 65 && newNick[i] <= 90) ||
			(newNick[i] >= 97 && newNick[i] <= 122))
			continue;
		else
		{
			return numericResponse("newNick cannot have special chars!", 0, "432");
		}
	}

	std::list<User *> users = _server.getUsers();
	std::list<User *>::iterator it = users.begin();
	for (; it != users.end(); ++it)
	{
		if ((*it)->getNick() == newNick)
			return numericResponse("Nick already in use!", 0, "433");
	}
	if (_user.getNick() != "")
		_server.messageAll(":" + _user.getNick() + " NICK " + newNick);
	else
		_server.messageAll(":127.0.0.1 001 all :" + newNick + " joined the server");
	_user.setNick(newNick);
}

void Command::ft_user()
{
	if (_options.size() != 4)
		return numericResponse("usage: /USER <username> <hostname> <servername> <realname>", 0, "461");
	if (!_user.getUsername().empty())
		return numericResponse("You are already registered", 0, "468");
	_user.setUsername(_options[0]);
	_user.setRealname(_options[3]);
	// std::string ack = "USER " + _options[0] + " 0 * " + _options[3];
	// userResponse(ack);
	// send(_user.getSocket(), ack.c_str(), strlen(ack.c_str()), 0);
}

// void Command::ft_quit()
// {
// 	std::string ack = "Quit";
// 	if (_options.size() > 0)
// 	{
// 		ack += ": ";
// 		for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
// 			ack += *it + " ";
// 	}
// 	std::cout << ack << std::endl;
// 	// _server.deleteUser(&this->_user);
// 	// _server.serverResponse(ack);
// }

void Command::ft_oper()
{
	if (_options.size() != 2)
		return numericResponse("usage: /OPER <nickname> <oper_password>", 0, "461");
	User *u = _server.getUserByNick(_options[0]);
	if (u == nullptr)
		return numericResponse("There's no user with this nick!", 0, "431");
	if (u->isOper())
		return numericResponse(_options[0] + " is an operator already!", 0, "610");
	if (_options[1] == OPER_PASS)
		u->setOper();
	else
		return numericResponse("Wrong password", 0, "464");
	_server.messageAllBut(":127.0.0.1 001 all :" + _options[0] + " became an operator!", u->getSocket());
	numericResponse("You became an operator. With great power comes great responsibility!", u->getSocket(), "381");
}

void Command::ft_exception(std::string s)
{
	if (s == "nick")
		numericResponse("Provide a nick to execute commands", 0, "451");
	if (s == "pass")
		numericResponse("Provide a password to execute commands", 0, "451");
}

// https://www.alien.net.au/irc/irc2numerics.html
void Command::numericResponse(std::string message, int socket, std::string resnum)
{
	std::string nick = _user.getNick();
	if (!nick.length())
		nick = "unknown";
	std::string res = ":127.0.0.1 " + resnum + " " + nick + " ";
	res += ":" + message + "\r\n";
	if (!socket)
		socket = _user.getSocket();
	if (send(socket, res.c_str(), strlen(res.c_str()), 0) == -1)
		throw std::runtime_error(strerror(errno));
}