#include "../includes/Client.hpp"

Client::Client()
{
    isLogged = false;
    is_irc = false;
}

Client::Client(int fd): fd(fd)
{
    isLogged = false;
    is_irc = false;
}

Client::Client(const Client &obj)
{
    *this = obj;
}

Client::~Client()
{
}

Client &Client::operator=(const Client &obj)
{
    this->nickname = obj.nickname;
    this->username = obj.username;
    this->fd = obj.fd;
    this->is_msg = obj.is_msg;
    this->is_irc = obj.is_irc;
    return(*this);
}

int Client::getFd() const
{
    return (this->fd);
}

int Client::getIsMsg() const
{
    return (this->is_msg);
}

int Client::getIsLogged() const
{
    return (this->isLogged);
}

std::string Client::getNick() const
{
    return (this->nickname);
}

std::string Client::getUser() const
{
    return (this->username);
}

std::string Client::getHost() const
{
    return (this->hostname);
}

bool Client::getIrc()
{
    return is_irc;
}

void Client::setIrc(bool val)
{
    is_irc = val;
}

void Client::setNick(std::string str)
{
    if (str.length() > 32)
    {
        std::cout << "Error: nick over 32 characters, if already exists 30 :: " << str.length() << std::endl;
        return ;
    }
    nickname = str;
}

void Client::setUser(std::string str)
{
    if (str.length() > 32)
    {
        std::cout << "Error: user over 32 characters, if already exists 30 :: " << str.length() << std::endl;
        return ;
    }
    username = str;
}

int Client::getId() const
{
    return (this->id);
}

void Client::setIsMsg(int i)
{
    is_msg = i;
}

void Client::setFd(int fd)
{
    this->fd = fd;
}

void Client::setId(int id)
{
    this->id = id;
}

void Client::setIsLogged(bool log)
{
    this->isLogged = log;
}