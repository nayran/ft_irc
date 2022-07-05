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

private:
    std::string _name;
    std::list<User *> _users;
    std::string _password;
    // Server *_server;
};

#include "serverClass.hpp"
#include "userClass.hpp"

#endif
