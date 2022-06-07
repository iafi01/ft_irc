#include "../includes/Client.hpp"

Client::Client()
{
}

Client::Client(int fd): fd(fd)
{
}

Client::Client(const Client &obj)
{
    *this = obj;
}

Client &Client::operator=(const Client &obj)
{
    this->nickname = obj.nickname;
    this->username = obj.username;
    this->fd = obj.fd;
    this->id = obj.id;
    this->is_msg = obj.is_msg;
    return(*this);
}

int Client::getFd() const
{
    return (this->fd);
}

int Client::getId() const
{
    return (this->id);
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

void Client::setIsMsg(int i)
{
    is_msg = i;
}
