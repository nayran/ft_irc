#include "commandClass.hpp"

Command::Command(std::string buff, int clisocket, Server &server) : _server(server), _user(*server.getUserBySocket(clisocket))
{
	parse(buff);
	run();
};

Command::~Command(){};

std::string to_upper(std::string s)
{
	int x = -1;
	while (s[++x] != '\0')
	{
		s[x] = std::toupper(s[x]);
	}
	return s;
}

void Command::parse(std::string buff)
{
	if (buff[0] == '/')
		buff.erase(0, 1);
	buff.erase(std::remove(buff.begin(), buff.end(), '\n'), buff.end());
	buff.erase(std::remove(buff.begin(), buff.end(), '\r'), buff.end());

	size_t pos = 0;
	while ((pos = buff.find(' ')) != std::string::npos)
	{
		_options.push_back(buff.substr(0, pos));
		buff.erase(0, pos + 1);
	}
	_options.push_back(buff.substr(0, buff.size()));
	_command = to_upper(*_options.begin());
	_options.erase(_options.begin());
	if (_options.size() > 0)
	{
		if ((*_options.begin())[0] == ':')
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
	else if (_command == "QUIT")
		ft_quit();
	else if (_command == "HELP")
		ft_help();
	else if (_user.isAuth())
	{
		if (_command == "NICK")
			ft_nick();
		else if (!_user.getNick().empty())
		{
			if (_command == "USER")
				ft_user();
			else if (!_user.getUsername().empty())
			{
				if (_command == "OPER")
					ft_oper();
				else if (_command == "JOIN")
					ft_join();
				else if (_command == "PART")
					ft_part();
				else if (_command == "NAMES")
					ft_names();
				else if (_command == "PRIVMSG")
					ft_privmsg();
				else if (_command == "KICK")
					ft_kick();
				else if (_command == "MODE")
					ft_mode();
				else if (_command == "WHO")
					ft_who();
				else if (_command == "NOTICE")
					ft_notice();
				else
					numericResponse(_command + " :Unknown command", "421");
			}
			else
				return numericResponse("provide a user: /USER <username> <hostname> <servername> <realname>", "431");
		}
		else
			return numericResponse("provide a nick: /NICK <newNick>", "431");
	}
	else
		return numericResponse("provide a password: /PASS <password>", "464");
}

void Command::ft_notice()
{
	if (_options.size() == 0 || _options[0].empty())
		return;
	if (_options.size() == 1 || _options[1].empty())
		return;
	User sender = _user;
	User *receiver = _server.getUserByNick(_options[0]);
	if (!receiver)
		return;
	std::string message;
	for (std::vector<std::string>::iterator it = _options.begin() + 1; it != _options.end(); it++)
	{
		message += *it + " ";
	}
	if (message[0] == ':')
		message.erase(0, 1);
	std::string res = ":" + sender.getNick() + " PRIVMSG " + receiver->getNick() + " :" + message;
	receiver->messageUser(res);
}

void Command::ft_help()
{
	numericResponse("You must provide Pass, Nick and User informations", "704");
	numericResponse("Available commands: NICK, USER, PASS, OPER, JOIN, PART, NAMES, PRIVMSG, KICK, MODE, WHO, QUIT, HELP", "705");
	numericResponse("usage: /<command>", "706");
}

void Command::ft_quit()
{
	std::string message;
	for (std::vector<std::string>::iterator it = _options.begin(); it != _options.end(); it++)
	{
		message += *it + " ";
	}
	_server.messageAll(":" + _user.getNick() + " QUIT :" + message);
	_server.deleteUser(_user.getSocket());
}

void Command::ft_who()
{
	if (_options.size() > 2)
		return numericResponse("usage: /WHO [<name> [<o>]]", "461");
	std::list<User *> users = _server.getUsers();
	std::string res;
	if (_options.size() == 0 || _options.begin()->empty())
	{
		for (std::list<User *>::iterator it = users.begin(); it != users.end(); it++)
			numericResponse((*it)->getRealname(), "352", 0, (*it)->getUsername() + " 0 * " + (*it)->getNick());
	}
	else
	{
		for (std::list<User *>::iterator it = users.begin(); it != users.end(); it++)
		{
			res = (*it)->getUsername() + " 0 * " + (*it)->getNick();
			if (res.find(_options[0], 0) != std::string::npos || (*it)->getRealname().find(_options[0], 0) != std::string::npos)
			{
				if (_options.size() == 2 && _options[1] == "o" && (*it)->isOper())
					numericResponse((*it)->getRealname(), "352", 0, res);
				else if (_options.size() == 1 || _options[1].empty())
					numericResponse((*it)->getRealname(), "352", 0, res);
			}
		}
	}
	numericResponse("End of /WHO list", "315", 0);
}

void Command::ft_mode()
{
	std::string limit;
	std::ostringstream convert;
	convert << CHANNEL_LIMIT;
	limit = convert.str();
	if (_options.size() > 0 && !_options[0].empty())
		numericResponse("", "324", 0, _options[0] + " l " + limit, 0);
}

void Command::ft_kick()
{
	if (_options.size() < 2 || _options[0].empty() || _options[1].empty())
		return numericResponse("usage: /KICK <channel> <client> [<message>]", "461");
	if (_options[0][0] != '#')
		_options[0] = '#' + _options[0];
	Channel *channel = _server.getChannelByName(_options[0]);
	if (!channel)
		return numericResponse("channel doesn't exist", "403", 0, _options[0]);
	if (!_user.isOper())
		return numericResponse("You need privileges to kick someone.", "482", 0, channel->getName());
	if (!channel->getUserByNick(_user.getNick()))
		return numericResponse("you need to join the channel to send any command to it", "442", 0, channel->getName());
	User *kickedUser = _server.getUserByNick(_options[1]);
	if (!kickedUser)
		return numericResponse(" is not in channel", "441", 0, _options[0] + " " + _options[1]);
	std::string message;
	for (std::vector<std::string>::iterator it = _options.begin() + 2; it != _options.end(); it++)
	{
		message += *it + " ";
	}
	if (message[0] == ':')
		message.erase(0, 1);
	std::string res = ":" + _user.getNick() + " KICK " + channel->getName() + " " + kickedUser->getNick() + " " + message;
	kickedUser->deleteChannel(channel);
	channel->deleteUser(kickedUser);
	kickedUser->messageUser(res);
	channel->messageChannel(res);
}

void Command::ft_privmsg()
{
	if (_options.size() == 0 || _options[0].empty())
		return numericResponse("You should give a Nick", "411");
	if (_options.size() == 1 || _options[1].empty())
		return numericResponse("You should write a message", "411");
	User sender = _user;
	if (_options[0][0] != '#')
	{
		User *receiver = _server.getUserByNick(_options[0]);
		if (!receiver)
			return numericResponse("Nick not found", "401");
		std::string message;
		for (std::vector<std::string>::iterator it = _options.begin() + 1; it != _options.end(); it++)
		{
			message += *it + " ";
		}
		if (message[0] == ':')
			message.erase(0, 1);
		std::string res = ":" + sender.getNick() + " PRIVMSG " + receiver->getNick() + " :" + message;
		receiver->messageUser(res);
	}
	else
	{
		Channel *channel = _server.getChannelByName(_options[0]);
		if (!channel)
			return numericResponse("Channel not found", "401");
		if (!channel->getUserByNick(_user.getNick()))
			return numericResponse("you need to join the channel to send a message to it", "442", 0, channel->getName());
		std::string message;
		for (std::vector<std::string>::iterator it = _options.begin() + 1; it != _options.end(); it++)
		{
			message += *it + " ";
		}
		if (message[0] == ':')
			message.erase(0, 1);
		std::string res = ":" + sender.getNick() + " PRIVMSG " + channel->getName() + " :" + message;
		channel->messageChannelBut(res, sender.getSocket());
	}
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
		if (channel)
		{
			std::list<User *> users = channel->getUsers();
			std::string res;
			for (std::list<User *>::iterator uit = users.begin(); uit != users.end(); uit++)
			{
				res += (*uit)->getNick() + " ";
			}
			if ((*it)[0] == '#')
				(*it).erase(0, 1);
			numericResponse(res, "353", 0, "= " + *it);
		}
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
		if ((*it)[0] != '#')
			*it = '#' + *it;
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
	if ((_options[0] == "vila56" || _options[0] == "#vila56") && (_user.getNick() != "jhow" && _user.getNick() != "nayran" && _user.getNick() != "edu" && _user.getNick() != "marcus" && _user.getNick() != "matthos"))
		return numericResponse("Only special villagers can enter this one!", "482");
	Channel *channel = _server.getChannelByName(_options[0]);
	if (!channel)
	{
		channel = new Channel(_options[0], _options[1]);
		_server.addChannel(channel);
	}
	if (_options[1] != channel->getPassword())
		return numericResponse("Wrong channel password", "464");
	else
	{
		if (channel->getUsers().size() == CHANNEL_LIMIT)
			return numericResponse("channel is full", "471", 0, channel->getName());
		_user.addChannel(channel);
	}
	channel->messageChannel(":" + _user.getNick() + " JOIN " + channel->getName());
	std::list<User *> users = channel->getUsers();
	std::string res;
	for (std::list<User *>::iterator uit = users.begin(); uit != users.end(); uit++)
	{
		res += (*uit)->getNick() + " ";
	}
	std::string channelName = channel->getName();
	if (channelName[0] == '#')
		channelName.erase(0, 1);
	numericResponse(res, "353", 0, "= " + channelName);
	numericResponse("End of /NAMES list", "366", 0, channelName);
}

void Command::ft_pass()
{
	if (_options.size() != 1 || _options[0] == "")
		return numericResponse("usage: /PASS <password>", "461");
	if (_user.isAuth())
		return numericResponse("You are already registered", "462");
	if (_options[0] == _server.getPassword())
	{
		_user.auth();
		return numericResponse("Password OK", "338");
	}
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

void Command::ft_oper()
{
	if (_options.size() != 2)
		return numericResponse("usage: /OPER <nickname> <oper_password>", "461");
	User *u = _server.getUserByNick(_options[0]);
	if (!u)
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
	// std::cout << res << std::endl;
	if (send(socket, res.c_str(), strlen(res.c_str()), 0) == -1)
		throw std::runtime_error("Couldn't SEND socket");
}
