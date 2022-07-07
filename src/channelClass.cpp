#include "channelClass.hpp"

Channel::Channel(std::string name, std::string password)
{
    _name = name;
    _password = password;
    _topic = name;
}

Channel::~Channel()
{
}

std::list<User *> Channel::getUsers()
{
    return _users;
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

void Channel::messageChannel(std::string message)
{
    if (message.find("\r\n"))
        message += "\r\n";
    for (std::list<User *>::iterator it = _users.begin(); it != _users.end(); ++it)
    {
        if (send((*it)->getSocket(), message.c_str(), strlen(message.c_str()), 0) == -1)
            throw std::runtime_error("Couldn't SEND messageChannel");
    }
}

void Channel::messageChannelBut(std::string message, int socket)
{
    if (message.find("\r\n"))
        message += "\r\n";
    for (std::list<User *>::iterator it = _users.begin(); it != _users.end(); ++it)
    {
        if ((*it)->getSocket() != socket)
        {
            if (send((*it)->getSocket(), message.c_str(), strlen(message.c_str()), 0) == -1)
                throw std::runtime_error("Couldn't SEND messageChannelBut");
        }
    }
}

User *Channel::getUserByNick(std::string name)
{
    for (std::list<User *>::iterator it = _users.begin(); it != _users.end(); ++it)
    {
        if (name == (*it)->getNick())
            return *it;
    }
    return NULL;
}

void Channel::deleteUser(User *user)
{
    for (std::list<User *>::iterator it = _users.begin();
         it != _users.end(); ++it)
    {
        if (*it == user)
        {
            _users.erase(it);
            return;
        }
    }
}