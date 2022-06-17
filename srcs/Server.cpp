#include "../includes/Server.hpp"

std::vector<std::string> ft_split(std::string toSplit, std::string toFind)
{
	std::vector<std::string> splitted;
	while(toSplit.size())
	{
		unsigned long index = splitted.find(toFind);
		if (index!=std::string::npos)
		{
			if (index != 0)
				splitted.push_back(toSplit.substr(0, index));
			toSplit = toSplit.substr(index + toFind.size());
		}
		else
		{
			splitted.push_back(toSplit);
			toSplit = "";
		}
	}
	return (splitted);
}

void Server::parse_commands(Client *client, char *buf, int valrecv)
{
	std::string clientCommand;
	std::vector<std::string> splitted;
	clientCommand.assign(buf, valrecv);

	splitted = ft_split(clientCommand, " ");
	if(!strncmp(buf, "QUIT", 4) || !strncmp(buf, "quit", 4))
		quit_cmd();
	else if(!strncmp(buf, "INVITE", 6) || !strncmp(buf, "invite", 6))
		invite_cmd();
	else if(!strncmp(buf, "KICK", 4) || !strncmp(buf, "kick", 4))
		kick_cmd();
	else if(!strncmp(buf, "JOIN", 4) || !strncmp(buf, "join", 4))
		join_cmd();
	else if(!strncmp(buf, "OP", 2) || !strncmp(buf, "op", 2))
		op_cmd();
	else if(!strncmp(buf, "DEOP", 4) || !strncmp(buf, "deop", 4))
		deop_cmd();
	else if(!strncmp(buf, "HALFOP", 6) || !strncmp(buf, "halfop", 6))
		half_cmd();
	else if(!strncmp(buf, "DEHALFOP", 8) || !strncmp(buf, "dehalfop", 8))
		dehalf_cmd();
	else if(!strncmp(buf, "BAN", 3) || !strncmp(buf, "ban", 3))
		ban_cmd();
	else if(!strncmp(buf, "UNBAN", 5) || !strncmp(buf, "unban", 5))
		unban_cmd();
	else if(!strncmp(buf, "VOICE", 5) || !strncmp(buf, "voice", 5))
		voice_cmd();
	else if(!strncmp(buf, "UNVOICE", 7) || !strncmp(buf, "unvoice", 7))
		unvoice_cmd();
	else
	{

	}

}

//private used by contructors
void Server::setup_server(int port, std::string password)
{
    bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(2130706433);
	serveraddr.sin_port = htons(port);
	pass = password;

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
	char buf[512];
	int valrecv = 0;
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
					if(valrecv = recv(fd, &buf, 1, 0) < 0)
						fatal();
					else if(valrecv = recv(fd, &buf, 1, 0) == 0)
					{
						sprintf(this->server_buffer, "server: client %d just left\n");
						delete &getClient(fd);
						send_all(this->server_buffer, fd);
						FD_CLR(fd, &this->curr_fds);
						close(fd);
						break;
					}
					else
					{
						buf[valrecv] = '\0';
						Client *client = &getClient(fd);
						/*if(client->getIsMsg())
						{
							sprintf(this->server_buffer, "client %d: ", client->getId());
							send_all(this->server_buffer, getClient(fd));
						}
						client->setIsMsg(c == '\n');
						send_all(&c, getClient(fd));*/
						parse_commands(client, buf, valrecv);
					}
				}
			}
		}
	}
}

void Server::fatal()
{
	std::perror(ERR_FAT);
	exit(1);
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
	if (this != &obj)
		*this = obj;
	return (*this);
}

void Server::accept_client(int sockfd)
{
	int new_fd;
	if((new_fd = accept(this->sockfd, NULL, NULL)) < 0)
		fatal();
	sprintf(this->server_buffer, "server: client %d just arrived\n", new_fd);
	send_all(this->server_buffer, getClient(sockfd));
	FD_SET(new_fd, &this->curr_fds);
	client_map.insert(std::make_pair(new_fd, getClient(new_fd)));
}

void Server::send_all(std::string mex, Client sender)
{
	int i = 0;
	while(clients[i])
	{
		if(clients[i] != &sender && FD_ISSET(clients[i]->getFd(), &write_fds))
		{
			if(send(clients[i]->getFd(), mex.c_str(), mex.length(), 0) < 0)
				fatal();
		}
		i++;
	}
}

//setters
void Server::setDate()
{
	time_t now = time(0);
	time_string = ctime(&now);
}

void Server::setPass(std::string new_pass)
{
	this->pass = new_pass;
}

//getters
int Server::get_port() const
{
	return (port);
}

int Server::get_sock() const
{
	return (sockfd);
}

int Server::get_max_id() const
{
	return (max_id);
}

std::string Server::get_pass() const
{
	return (pass);
}

std::string Server::getDate() const
{
	return (time_string);
}

int get_max_fd(int sockfd)
{

}

void parse_commands(Client *client, std::string cmd)
{

}
void quit_cmd()
{

}
void mode_cmd()
{

}
void invite_cmd()
{

}
void topic_cmd()
{

}
void kick_cmd()
{

}

void join_cmd()
{

}

//clients and channels management by server
Client Server::getClient(int sockfd)
{
	for(std::map<int, Client*>::iterator it = client_map.begin(); it != client_map.end(); it++)
	{
		if(sockfd == it->first)
			return (*it->second);
	}
}

Channel Server::getChannel(std::string nameCh)
{
	for(std::map<std::string, Channel*>::iterator it = channel_map.begin(); it != channel_map.end(); it++)
	{
		if(nameCh == it->first)
			return (*it->second);
	}
}