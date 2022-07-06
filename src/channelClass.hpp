#ifndef CHANNELCLASS_HPP
#define CHANNELCLASS_HPP

#include "ft_irc.hpp"

class Server;
class User;

class Channel
{
public:
    Channel(std::string name, std::string password);
    ~Channel();
    std::string getName();
    void addUser(User *user);
    std::string getPassword();
    std::list<User *> getUsers();
    User *getUserByNick(std::string name);
    void messageChannel(std::string message);
    void deleteUser(User *user);

private:
    std::string _name;
    std::list<User *> _users;
    std::string _password;
    std::string _topic;
    // Server *_server;
};

#include "serverClass.hpp"
#include "userClass.hpp"

#endif
