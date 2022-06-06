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

Client Client::get_client(int fd)
{

}

int Client::get_fd() const
{
    return (this->fd);
}

int Client::get_id() const
{
    return (this->id);
}

int Client::get_is_msg() const
{
    return (this->is_msg);
}

std::string Client::get_nick() const
{
    return (this->nickname);
}

std::string Client::get_user() const
{
    return (this->username);
}