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

	void ft_userexcept();
	void ft_passexcept();

	void shutdown(int sig);

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

	void parse(std::string buff);
	void run();
};


#include "serverClass.hpp"
#include "userClass.hpp"

#endif
