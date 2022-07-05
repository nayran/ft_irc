#include "channelClass.hpp"

Channel::Channel(std::string name, std::string password)
{
    _name = name;
    _password = password;
}

Channel::~Channel()
{
}

std::string Channel::getName()
{
    return _name;
}

void Channel::addUser(User *user)
{
    for (std::list<User *>::iterator it = _users.begin();
         it != _users.end(); ++it)
    {
        if ((*it)->getNick() == user->getNick())
            return;
    }
    _users.push_back(user);
}

std::string Channel::getPassword()
{
    return _password;
}

void Channel::channelResponse(std::string ack)
{
    for (std::list<User *>::iterator it = _users.begin(); it != _users.end(); ++it)
    {
        // :nayran2 JOIN #dsf
        // std::string res = ":127.0.0.1 001 ";
        std::string res = ":" + (*it)->getNick() + " JOIN " + this->getName() + "\r\n";
        send((*it)->getSocket(), res.c_str(), strlen(res.c_str()), 0);
        std::cout << res << std::endl;
        std::cout << ack << std::endl;
    }
}
