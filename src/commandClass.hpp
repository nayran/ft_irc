#ifndef COMMANDCLASS_HPP
#define COMMANDCLASS_HPP

#include "ft_irc.hpp"
#include <sstream>
#include <algorithm>

class Server;
class User;

class Command
{
public:
	Command(std::string buff, int client, Server &server);
	~Command();

	void ft_exception(std::string s);
	void shutdown(int sig);
	bool specialChar(std::string s, size_t i = 0);
	void numericResponse(std::string message, std::string resnum, int socket = 0, std::string moreopts = "", int colon = 1);

private:
	std::vector<std::string> _options;
	std::string _command;
	Server &_server;
	User &_user;

	void ft_nick();
	void ft_user();
	void ft_quit();
	void ft_pass();
	void ft_oper();
	void ft_join();
	void ft_part();
	void ft_names();
	void ft_privmsg();
	void ft_kick();

	void parse(std::string buff);
	void run();
};

#include "serverClass.hpp"
#include "userClass.hpp"

#endif
