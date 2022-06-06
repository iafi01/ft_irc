#include "../includes/Server.hpp"

//private used by contructors
void Server::setup_server(int port, std::string pass)
{
    bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(2130706433);
	serveraddr.sin_port = htons(port);

	this->sockfd = -1;
	if((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		fatal();
	if(bind(this->sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		fatal();
	if(listen(this->sockfd, 0) < 0)
		fatal();
}

void Server::start_server()
{
	FD_ZERO(&this->curr_fds);
	FD_SET(this->sockfd, &this->curr_fds);
	while(1)
	{
		this->write_fds = this->read_fds = this->curr_fds;
		if(select(get_max_fd(this->sockfd) + 1, &this->read_fds, &this->write_fds, NULL, NULL) < 0)
			continue;
		for(int fd = 0; fd <= get_max_fd(this->sockfd); fd++)
		{
			if(FD_ISSET(fd, &this->read_fds))
			{
				if(fd == this->sockfd)
				{
					accept_client(this->sockfd);
					break;
				}
				else
				{
					char c;
					if(recv(fd, &c, 1, 0) <= 0)
					{
						sprintf(this->server_buffer, "server: client %d just left\n", del_client(fd, this));
						send_all(this->server_buffer, strlen(s->server_buffer), fd, this);//da vedere
						FD_CLR(fd, &this->curr_fds);
						close(fd);
						break;
					}
					else
					{
						Client *client = get_client(fd, this);//Da vedere
						if(client->is_msg)
						{
							sprintf(this->server_buffer, "client %d: ", client->id);
							send_all(this->server_buffer, strlen(s->server_buffer), fd, this);
						}
						client->new_msg = (c == '\n');
						send_all(&c, 1, fd, this);
					}
				}
			}
		}
	}
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



void Server::accept_client(int sockfd)
{
	int new_fd;
	if((new_fd = accept(this->sockfd, NULL, NULL)) < 0)
		fatal();
	sprintf(this->server_buffer, "server: client %d just arrived\n", new_client(new_fd, this));
	send_all(thisserver_buffer, strlen(this->server_buffer), new_fd, getClient(sockfd));
	FD_SET(new_fd, &this->curr_fds);
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

//clients and channels management by server
Client Server::getClient(int sockfd)
{
	for(std::map<int, Client*>::iterator it = client_map.begin(); it != client_map.end(); it++)
	{
		Client* tmp = client_map.begin();
		if(sockfd == tmp->first)
			return *(client_map.at());
		tmp++;
	}
}