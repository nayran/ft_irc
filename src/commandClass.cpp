#include "commandClass.hpp"

Command::Command(std::string buff, int clisocket, Server &server) : _server(server), _user(*server.getUserBySocket(clisocket))
{
	parse(buff);
	run();
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

// https://simple.wikipedia.org/wiki/List_of_Internet_Relay_Chat_commands
// https://datatracker.ietf.org/doc/html/rfc1459
void Command::run()
{
	if (_command == "CAP")
		numericResponse("CAP * ACK multi-prefix", "");
	else if (_command == "PONG")
		return;
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
			else if (_command == "PART")
				ft_part();
			else if (_command == "NAMES")
				ft_names();
			else if (_command == "MODE")
				ft_mode();
			else
			{
				// std::cout << "else" << std::endl;
				std::string ack = "Unknown command: " + _command;
				std::cout << ack << std::endl;
				for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
					std::cout << " " << *it;
				std::cout << std::endl;
			}
		}
		else
			ft_exception("nick");
	}
	else
		ft_exception("pass");
}

// https://www.alien.net.au/irc/chanmodes.html
// https://datatracker.ietf.org/doc/html/rfc1459#section-4.2.3
void Command::ft_mode()
{
	numericResponse("", "324", 0, _options[0] + " b,k,l,imnpst", 0);
}

void Command::ft_names()
{
	if (_options.size() < 1 || _options[0].empty())
	{
		if (_options[0].empty())
			_options.erase(_options.begin());
		std::list<Channel *> channels = _server.getChannels();
		for (std::list<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
		{
			_options.push_back((*it)->getName());
		}
	}
	for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); ++it)
	{
		Channel *channel = _server.getChannelByName(*it);
		if (!channel)
		{
			numericResponse("End of /NAMES list", "366", 0, *it);
			continue;
		}
		std::list<User *> users = channel->getUsers();
		std::string res;
		for (std::list<User *>::iterator uit = users.begin(); uit != users.end(); uit++)
		{
			res += (*uit)->getNick() + " ";
		}
		numericResponse(res, "353", 0, "= " + *it);
		numericResponse("End of /NAMES list", "366", 0, *it);
	}
}

void Command::ft_part()
{
	if (_options.size() < 1 || _options[0] == "")
		return numericResponse("usage: /PART <channels>", "461");
	std::vector<std::string>::iterator it = _options.begin();
	while (it != _options.end())
	{
		if ((*it)[0] == ':')
			it->erase(0, 1);
		Channel *channel = _server.getChannelByName(*it);
		if (!channel)
			numericResponse("channel doesn't exist", "403", 0, *it);
		else if (!channel->getUserByNick(_user.getNick()))
			numericResponse("you need to join the channel to leave it", "442", 0, *it);
		else
		{
			std::string ack = ":" + _user.getNick() + " PART " + *it;
			channel->messageChannel(ack);
			_user.deleteChannel(channel);
			channel->deleteUser(&_user);
		}
		it++;
	}
}

// https://stackoverflow.com/questions/43399621/join-command-for-irc-protocol?rq=1
void Command::ft_join()
{
	if (*_options.begin() == "#" || _options.size() < 1 || _options.size() > 2)
		return numericResponse("usage: /JOIN <channel> [<channel_password>]", "461");
	if (specialChar(*_options.begin(), 1))
		return numericResponse("Channel name cannot have special chars!", "403");
	if (_options.size() == 1)
		_options.push_back("");
	Channel *channel = _server.getChannelByName(_options[0]);
	if (!channel)
	{
		channel = new Channel(_options[0], _options[1]);
		_server.addChannel(channel);
	}
	if (_options[1] != channel->getPassword())
		return numericResponse("Wrong channel password", "464");
	else
		_user.addChannel(channel);
	// _user.messageUser(":" + _user.getNick() + " JOIN " + channel->getName());
	channel->messageChannel(":" + _user.getNick() + " JOIN " + channel->getName());
}

void Command::ft_pass()
{
	if (_options.size() != 1 || _options[0] == "")
		return numericResponse("usage: /PASS <password>", "461");
	if (_user.isAuth())
		return numericResponse("You are already registered", "462");
	if (_options[0] == _server.getPassword())
		_user.auth();
	else
		return numericResponse("Wrong password", "461");
}

void Command::ft_nick()
{
	std::string newNick = _options[0];
	if (_options.size() != 1)
		return numericResponse("usage: /NICK <newNick>", "431");
	if (newNick.length() < 3)
		return numericResponse("newNick must have at least 3 chars!", "432");
	if (newNick == "anonymous" || newNick == "unknown")
		return numericResponse("This nick cannot be used!", "432");
	if (specialChar(newNick))
		return numericResponse("newNick cannot have special chars!", "432");
	std::list<User *> users = _server.getUsers();
	std::list<User *>::iterator it = users.begin();
	for (; it != users.end(); ++it)
	{
		if ((*it)->getNick() == newNick)
			return numericResponse("Nick already in use!", "433");
	}
	// if (_user.getNick() != "")
	_server.messageAll(":" + _user.getNick() + " NICK " + newNick);
	// else
	// 	_server.messageAll(":127.0.0.1 001 all :" + newNick + " joined the server");
	_user.setNick(newNick);
}

void Command::ft_user()
{
	if (_options.size() != 4)
		return numericResponse("usage: /USER <username> <hostname> <servername> <realname>", "461");
	if (!_user.getUsername().empty())
		return numericResponse("You are already registered", "468");
	_user.setUsername(_options[0]);
	_user.setRealname(_options[3]);
	std::string ack = "USER " + _options[0] + " 0 * " + _options[3] + "\r\n";
	send(_user.getSocket(), ack.c_str(), strlen(ack.c_str()), 0);
	numericResponse("Welcome to Nayran's ft_irc " + _user.getNick(), "001");
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
		return numericResponse("usage: /OPER <nickname> <oper_password>", "461");
	User *u = _server.getUserByNick(_options[0]);
	if (u == nullptr)
		return numericResponse("There's no user with this nick!", "431");
	if (u->isOper())
		return numericResponse(_options[0] + " is an operator already!", "610");
	if (_options[1] == OPER_PASS)
		u->setOper();
	else
		return numericResponse("Wrong password", "464");
	_server.messageAllBut(":127.0.0.1 001 all :" + _options[0] + " became an operator!", u->getSocket());
	numericResponse("You became an operator. With great power comes great responsibility!", "381", u->getSocket());
}

void Command::ft_exception(std::string s)
{
	if (s == "nick")
		numericResponse("Provide a nick to execute commands", "451");
	if (s == "pass")
		numericResponse("Provide a password to execute commands", "451");
}

bool Command::specialChar(std::string s, size_t i)
{
	for (; i < s.length(); i++)
	{
		if (!(s[i] >= 48 && s[i] <= 57) &&
			!(s[i] >= 65 && s[i] <= 90) &&
			!(s[i] >= 97 && s[i] <= 122))
			return true;
	}
	return false;
}

// https://www.alien.net.au/irc/irc2numerics.html
// https://stackoverflow.com/questions/38753029/get-irc-command-response#38753693
void Command::numericResponse(std::string message, std::string resnum, int socket, std::string moreopts, int colon)
{
	std::string nick = _user.getNick();
	if (!nick.length())
		nick = "unknown";
	std::string res = ":127.0.0.1 " + resnum + " " + nick + " ";
	if (moreopts != "")
		res += moreopts + " ";
	if (colon)
		res += ":";
	res += message + "\r\n";
	if (!socket)
		socket = _user.getSocket();
	if (send(socket, res.c_str(), strlen(res.c_str()), 0) == -1)
		throw std::runtime_error(strerror(errno));
	std::cout << res << std::endl;
}