#include "../includes/Server.hpp"

bool Server::mode_cmd(Client *client, std::vector<std::string> splitted)
{
	std::string flag = splitted[2];												//+o, -o, +v, -v, +h, -h, +b, -b
	std::string channel_name = splitted[1];										//#<channel_name>
	std::vector<Client *> users = clientConvert(splitted);				//List of users
	if(compStr(flag, "+o"))	// /OP <nickname> works only inside a channel but the server receives the command as "MODE #miocanale +o <nickname>"
		op_cmd(client, channel_name, users);
	else if(compStr(flag, "-o"))
		deop_cmd(client, channel_name, users); // /DEOP <nickname> works only inside a channel but the server receives the command as "MODE #miocanale -o <nickname>"
	else if(compStr(flag, "+h"))
		half_cmd(client, channel_name, users); // /HALFOP <nickname> works only inside a channel but the server receives the command as "MODE #miocanale +h <nickname>"
	else if(compStr(flag, "-h"))
		dehalf_cmd(client, channel_name, users);	// /DEHALFOP <nickname> works only inside a channel but the server receives the command as "MODE #miocanale -h <nickname>"
	else if(compStr(flag, "+b"))
		ban_cmd(client, channel_name, splitted[3], topicConvert(splitted + 4));	// /BAN <nickname>, the server receives the command as "MODE #miocanale +b <nickname>"
	//Since ban supports different parameters called "Masks", the string that the server receives may differ
	else if(compStr(flag, "-b"))
		unban_cmd(client, channel_name, splitted[3]); // /UNBAN <nickname>, the server receives the command as "MODE #miocanale -b <nickname>"
	//Same as the ban command, unban is often used with different masks
	else if(compStr(flag, "+v"))
		voice_cmd(client, channel_name, users);
	else if(compStr(flag, "-v"))
		unvoice_cmd(client, channel_name, users);
}

std::vector<std::string> Server::parseBanMask(std::string banMask)
{
	std::vector<std::string> str1;
	std::vector<std::string> str2;
	std::vector<std::string> result;

	//Aggiungere controllo asterischi nei 3 if
	if (banMask.find("!") == std::string::npos)
	{
		return (ft_split(banMask, "!"));
	}
	else if (banMask.find("!") >= 0)
	{
		if (banMask.find("@") == std::string::npos)
		{
			return (ft_split(banMask, "!"));
		}
		else
		{
			str1 = ft_split(banMask, "!");
			str2 = ft_split(str1[1], "@");
			result.push_back(str1[0]);
			result.push_back(str2[0]);
			result.push_back(str2[1]);
			return (result);
		}
	}
}

void Server::op_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToOp)
{
	Channel *channel;

	if (!channel->isOp(admin))
		return ;
	channel = this->getChannel(channel_name);
	for (uint i = 0; i < clientToOp.size(); i++)
		if (!channel->op(clientToOp[i]))
			return ;
}

void Server::deop_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToDeOp)
{
	Channel *channel;

	if (!channel->isOp(admin))
		return ;
	channel = this->getChannel(channel_name);
	for (uint i = 0; i < clientToDeOp.size(); i++)
		if (!channel->deop(clientToDeOp[i]))
			return ;
}

void Server::half_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToHalfOp)
{
	Channel *channel;

	if (!channel->isOp(admin))
		return ;
	channel = this->getChannel(channel_name);
	for (uint i = 0; i < clientToHalfOp.size(); i++)
		if (!channel->halfOp(clientToHalfOp[i]))
			return ;
}

void Server::dehalf_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToDeHalfOp)
{
	Channel *channel;

	if (!channel->isOp(admin))
		return ;
	channel = this->getChannel(channel_name);
	for (uint i = 0; i < clientToDeHalfOp.size(); i++)
		if (!channel->deHalfOp(clientToDeHalfOp[i]))
			return ;
}

void Server::voice_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToVoice)
{
	Channel *channel;

	if (!channel->isHalfOp(admin))
		return ;
	channel = this->getChannel(channel_name);
	for (uint i = 0; i < clientToHalfOp.size(); i++)
		if (!channel->voiceOp(clientToHalfOp[i]))
			return ;
}

void Server::unvoice_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToUnVoice)
{
	Channel *channel;

	if (!channel->isHalfOp(admin))
		return ;
	channel = this->getChannel(channel_name);
	for (uint i = 0; i < clientToUnVoice.size(); i++)
		if (!channel->deVoiceOp(clientToUnVoice[i]))
			return ;
}


void Server::ban_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToBan, std::string reason)
{
	Channel *channel;

	if (!channel->isOp(admin))
		return ;
	channel = this->getChannel(channel_name);
	for (uint i = 0; i < clientToBan.size(); i++)
		if (!channel->ban(clientToBan[i]->getNick(), clientToBan[i]->getUser(), clientToBan[i]->getHost(), admin, reason))
			return ;
}

void Server::unban_cmd(Client *admin, std::string channel_name, std::string clientToUnBan)
{
	Channel *channel;

	if (!channel->isOp(admin))
		return ;
	channel = this->getChannel(channel_name);
	for (uint i = 0; i < clientToUnBan.size(); i++)
		if (!channel->unBan(clientToUnBan[i]->getNick(), clientToUnBan[i]->getUser(), clientToUnBan[i]->getHost(), admin))
			return ;
}

std::vector<Channel *> Server::channelConvert(std::vector<std::string> splitted)
{
	std::vector<Channel *> channel_list;
	for(std::map<std::string, Channel*>::iterator it = channel_map.begin(); it != channel_map.end(); it++)
	{
		for (uint i = 1; i < splitted.size(); i++)
		{
			if (splitted[i] == it->second)
				channel_list.push_back(*it->second);
		}
	}
	return (channel_list);
}

//se i nick passati sono nel server, crea un nuovo vettore di clients
std::vector<Client *> Server::clientConvert(std::vector<std::string> splitted)
{
	std::vector<Client *> new_clients;
	for (uint j = 0; j < clients.size() - 1; j++)
	{
		for (uint i = 1; i < splitted.size(); i++)
		{
			if (splitted[i] == clients[j]->getNick())
				new_clients.push_back(clients[j]);
		}
	}
	return (new_clients);
}

std::string topicConvert(std::vector<std::string> toConv)
{
	std::string result;
	for (uint i = 0; i < toConv.size(); i++)
	{
		result += toConv[i];
		if (i == toConv.size() - 1) //se é l'ultima stringa prima del nome del channel
			result += "\0";
		else
			result +=  " ";
	}	
	return (result);
}

std::string Server::toUpper(std::string toUp)
{
	std::transform(toUp.begin(), toUp.end(),toUp.begin(), ::toupper);
	return (toUp);
}

bool Server::compStr(std::string buffer, std::string str)
{
	if(!std::strncmp(buffer.c_str(), str.c_str(), str.length() + 1))
		return true;
	return false;
}

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
					char c;
					if((valrecv = recv(fd, &buf, 1, 0)) < 0)
						fatal();
					else if((valrecv = recv(fd, &buf, 1, 0)) == 0)
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
						if(client->getIsMsg() && buf[0] == '/')
						{
							sprintf(this->server_buffer, "client %d: ", client->getId());
							send_all(this->server_buffer, getClient(fd));
						}
						if (!parse_commands(client, buf, valrecv))
						{
							client->setIsMsg(c == '\n');
							send_all(&c, getClient(fd));
						}
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
	clients.push_back(getClient(new_fd));
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

// int Server::get_max_fd(int sockfd)
// {

// }

bool Server::parse_commands(Client *client, char *buf, int valrecv)
{
	std::string clientCommand;
	std::string::iterator it;
	std::vector<std::string> splitted;
	std::string aStr;
	clientCommand.assign(buf, (size_t)valrecv);

	splitted = ft_split(clientCommand, " ");
	aStr = toUpper(splitted[0]);
	if(compStr(aStr, "QUIT"))
		quit_cmd(client, splitted + 1);
	else if(compStr(aStr, "INVITE"))
		invite_cmd(clientConvert(splitted), splitted[splitted.size() - 1]);
	else if(compStr(aStr, "TOPIC"))
		topic_cmd(splitted[1] ,topicConvert(splitted + 2));
	else if(compStr(aStr, "KICK"))
	{
		if (splitted[3])
			kick_cmd(splitted[1], splitted[2], splitted[3]);
		else
			kick_cmd(splitted[1], splitted[2]);
	}
	else if(compStr(aStr, "JOIN"))
		join_cmd(client, splitted);
	else if(compStr(aStr, "WHO"))
		who_cmd();
	else if(compStr(aStr, "WHOIS"))
		whois_cmd();
	else if(compStr(aStr, "PRIVMSG"))
		privmsg_cmd()
	else if(compStr(aStr, "MODE"))
		mode_cmd(client, splitted);
	else if(compStr(aStr, "LEAVE"))
		leave_cmd(channelConvert(splitted));
	else if(compStr(aStr, "PASS"))
		leave_cmd(client, splitted[1]);
	else
		return false;
}

bool Server::quit_cmd(Client *client, std::vector<std::string> words)
{
	//quitta dal server e puoi mandare un messaggio (no fucking flags)
	int id = client->getFd();
	int fd = client->getId();
	std::string msg_quit;
	std::string msg;

	std::cout << "The disconnected host was named " << client->getUser() << std::endl;
	
	Channel* i = channel_map.begin()->second;
	while (i < channel_map.end()->second)
	{
		//deve mandare questo messaggio a tutti gli utenti di tutti i canali di cui facevi parte e uscire kickarti dai canali
		//iafi [~kvirc@Azzurra-3476AEA0.business.telecomitalia.it] has quit IRC: Quit: sono uscito

		
		//lista dei canali di cui fai parte
		//ciclati e con un send all
		for (uint j = 0; j < words.size(); j++) 
		{
			msg_quit += words[j];
			if (j < words.size() - 1)
				msg_quit += " ";
			else
				msg_quit += '\0';
		}
		std::vector<Client *> channel_clients = i->getClients();
		for (std::vector<Client *>::iterator l = channel_clients.begin(); l < channel_clients.end(); l++)
		{
			if (client == channel_clients[l])
			{
				//fa parte di questo canale
				msg += client->getUser() << " " << client->getHost() << " has quit IRC: Quit: " << msg_quit;
				send_all(msg, client);
				i->kickCmd(client, "quit");
			}
		}
		i++;
	}
	msg.clear();
	msg += "Server ERROR: :Closing Link: host" << client->getHost() << " " << "(Quit: " << msg_quit << ")";
	std::cout << msg << std::endl;
	//remove client from clients client_map
	msg.clear();
	client_map.erase(fd);
	clients.erase(id);
	close(fd);
	exit(0);
}

bool Server::privmsg_cmd(std::string mex, Client *receiver, Client *sender)
{
	//controllare sia sul server
	if(receiver != sender && FD_ISSET(receiver->getFd(), &write_fds))
	{
		if((receiver->getFd(), mex.c_str(), mex.length(), 0) < 0)
			fatal();
	}
	return true;
}

bool Server::invite_cmd(std::vector<Client *> invited, std::string channel_name)
{
	//gli inviti possono essere una lista di nomi
	//in funzione di questo controllo se quei client sono nel server
	//PS: vengono invitati solo se presenti

	Channel *channel;
	std::map<int, Client*>::iterator it;

	channel = this->getChannel(channel_name);
	it = client_map.begin();

	for (int i = 0; i < invited.lenght(); i++)
	{
		while(it != client_map.end())
		{
			if (invited[i] == *it->second)
				break;
			it++;
			if (it == client_map.end())
				return false;
		}
		if (channel->invite(invited[i]))
			return true;
		return false;
	}
}

bool Server::topic_cmd(std::string channel_name, std::string topic = "")
{
	Channel *channel;
	
	channel = this->getChannel(channel_name);
	if (topic == "")
	{
		channel->getTopic();
		//getTime e Sender
		return true;
	}
	else if (channel->setTopic(topic)) //setTime e Sender
		return true;
    return false;
}

bool Server::kick_cmd(std::string channel_name, std::string client_name, std::string reason = "")
{
	Channel *channel;

	channel = this->getChannel(channel_name);
	for (uint i = 0; i < clients.size(); i++)
		if (clients[i].getNick() == client_name)
			return (channel.kickCmd(clients[i], reason));
	return false;
}

bool Server::join_cmd(Client *client, std::string channel_name, std::string psw = "")
{
	//controlla che channel_name esiste nel map e accedi al second
	Channel *channel;
	
	channel = this->getChannel(channel_name);
	if (channel->connect(client, psw))
		return true;
    return false;
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