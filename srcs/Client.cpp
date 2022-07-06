#include "../includes/Client.hpp"

Client::Client()
{
    isLogged = false;
}

Client::Client(int fd): fd(fd)
{
    randomInfo();
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

bool Client::getLog() const
{
    return (this->isLogged);
}

void Client::setNick(std::string str)
{
    nickname = str;
}

void Client::setUser(std::string str)
{
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

void Client::randomInfo()
{
    int i = rand() % 9999 + 1;
    nickname.append("nick#" + std::to_string(i));
    username.append("user#" + std::to_string(i));
}