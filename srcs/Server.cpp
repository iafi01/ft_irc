#include "../includes/Server.hpp"

//private used by contructors
void Server::setup_server(int port, std::string pass)
{

}

void Server::start_server()
{

}

//public
Server::Server()
{

}

Server::~Server()
{

}

Server::Server(const int port, const std::string pass): port(port), pass(pass)
{
    setup_server(port, pass);
    start_server();
}

Server::Server(const Server &obj)
{
    *this = obj;
}

Server& Server::operator=(const Server &obj)
{

}



void Server::accept_client()
{

}

void Server::send_all(std::vector<Client*> receivers, std::string mex, Client sender)
{

}

//setters
void Server::setDate()
{

}

void Server::setPass()
{

}

//getters
int Server::get_port() const
{

}

int Server::get_sock() const
{

}

int Server::get_max_id() const
{

}

std::string Server::get_pass() const
{

}

std::string Server::getDate() const
{

}