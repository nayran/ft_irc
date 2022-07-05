#ifndef SERVERCLASS_HPP
#define SERVERCLASS_HPP

#include "ft_irc.hpp"
#include <netdb.h>
#include <exception>
#include <unistd.h>
#include <poll.h>
#include "channelClass.hpp"

class User;
class Command;

class Server
{
public:
	Server(std::string host, std::string port, std::string password);
	~Server();
	void setSockets();
	void init();
	void init2();
	int acceptUser();
	int receiveMessage(int clifd);
	User *getUserBySocket(int socket);
	User *getUserByNick(std::string nick);
	int getSocket();
	std::string getPassword();
	std::list<User *> getUsers();
	void messageAll(std::string ack);
	void messageAllBut(std::string ack, int socket);
	void deleteUser(User *_user);
	Channel *getChannelByName(std::string name);
	void addChannel(Channel *channel);

private:
	std::string _host;
	std::string _port;
	std::string _password;
	int _socket;
	std::vector<pollfd> _fdvec;
	std::list<User *> _users;
	std::list<Channel *> _channels;
};

#include "userClass.hpp"
#include "commandClass.hpp"

#endif
