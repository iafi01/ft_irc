#include "../includes/Server.hpp"

std::vector<Client*>::iterator Server::findIterClient(Client *client)
{
	std::vector<Client*>::iterator i;

    i = clients.begin();
    while (i != clients.end())
    {
        if ((*i) == client)
        {   
            return (i);
        }
        i++;
    }
    return std::vector<Client*>::iterator();
}

std::vector<std::string> ft_split(std::string toSplit, std::string toFind)
{
	std::vector<std::string> splitted;
	while(toSplit.size())
	{
		unsigned long index = toSplit.find(toFind);
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

void Server::mode_cmd(Client *client, std::vector<std::string> splitted)
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
	{
		std::vector<std::string> tmp;
		tmp.assign(splitted.begin() + 4, splitted.end());
		ban_cmd(client, channel_name, users, topicConvert(tmp));
	}	// /BAN <nickname>, the server receives the command as "MODE #miocanale +b <nickname>"
	//Since ban supports different parameters called "Masks", the string that the server receives may differ
	else if(compStr(flag, "-b"))
		unban_cmd(client, channel_name, users); // /UNBAN <nickname>, the server receives the command as "MODE #miocanale -b <nickname>"
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
	return std::vector<std::string>(); //return a void vector of strings
}

void Server::op_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToOp)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	if (!channel->isOp(admin))
	{
		msg += channel_name + ": You are not channel operator";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	for (uint i = 0; i < clientToOp.size(); i++)
		channel->op(clientToOp[i]);
}

void Server::deop_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToDeOp)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	if (!channel->isOp(admin))
	{
		msg += channel_name + ": You are not channel operator";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	for (uint i = 0; i < clientToDeOp.size(); i++)
		channel->deop(clientToDeOp[i]);
}

void Server::half_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToHalfOp)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	if (!channel->isOp(admin))
	{
		msg += channel_name + ": You are not channel operator";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	for (uint i = 0; i < clientToHalfOp.size(); i++)
		channel->halfOp(clientToHalfOp[i]);
}

void Server::dehalf_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToDeHalfOp)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	if (!channel->isOp(admin))
	{
		msg += channel_name + ": You are not channel operator";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	for (uint i = 0; i < clientToDeHalfOp.size(); i++)
		channel->deHalfOp(clientToDeHalfOp[i]);
}

void Server::voice_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToVoice)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	if (channel->isOp(admin) || channel->isHalfOp(admin))
	{
		for (uint i = 0; i < clientToVoice.size(); i++)
			channel->voiceOp(clientToVoice[i]);
	}
	else
	{
		msg += channel_name + ": You are not channel operator";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
}

void Server::unvoice_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToUnVoice)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	if (channel->isOp(admin) || !channel->isHalfOp(admin))
	{
		for (uint i = 0; i < clientToUnVoice.size(); i++)
			channel->deVoiceOp(clientToUnVoice[i]);
	}
	else
	{
		msg += channel_name + ": You are not channel operator";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
}


void Server::ban_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToBan, std::string reason)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	if (!channel->isOp(admin))
	{
		msg += channel_name + ": You are not channel operator";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	for (uint i = 0; i < clientToBan.size(); i++)
		channel->ban(admin, clientToBan[i]->getNick(), clientToBan[i]->getUser(), clientToBan[i]->getHost(), reason);
}

void Server::unban_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToUnBan)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	if (!channel->isOp(admin))
	{
		msg += channel_name + ": You are not channel operator";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	for (uint i = 0; i < clientToUnBan.size(); i++)
		channel->unBan(clientToUnBan[i]->getNick(), clientToUnBan[i]->getUser(), clientToUnBan[i]->getHost());
}

std::vector<Channel *> Server::channelConvert(std::vector<std::string> splitted)
{
	std::vector<Channel *> channel_list;
	for(std::map<std::string, Channel*>::iterator it = channel_map.begin(); it != channel_map.end(); it++)
	{
		for (uint i = 1; i < splitted.size(); i++)
		{
			if (splitted[i] == it->first)
				channel_list.push_back(it->second);
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

std::string Server::topicConvert(std::vector<std::string> toConv)
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
					//char c;
					if((valrecv = recv(fd, &buf, 512, 0)) < 0)
						fatal();
					else if((valrecv = recv(fd, &buf, 512, 0)) == 0)
					{
						sprintf(this->server_buffer, "server: client %d just left\n", fd);
						//delete getClient(fd);
						send_all(this->server_buffer, *getClient(fd));
						FD_CLR(fd, &this->curr_fds);
						close(fd);
						break;
					}
					else
					{
						Client *client = getClient(fd);
						if(client->getIsMsg() && buf[0] == '/')
						{
							sprintf(this->server_buffer, "client %d: ", client->getId());
							send_all(this->server_buffer, *getClient(fd));
						}
						if (!parse_commands(client, buf, valrecv))
						{
							client->setIsMsg(buf[valrecv] == '\n');
							send_all(buf, *getClient(fd));
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

Server::Server(const int port, const std::string pass)
{
	this->port = port;
	this->pass = pass;
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
	send_all(this->server_buffer, *getClient(sockfd));
	FD_SET(new_fd, &this->curr_fds);
	client_map.insert(std::make_pair(new_fd, getClient(new_fd)));
	clients.push_back(getClient(new_fd));
}

void Server::send_all(std::string mex, Client sender)		/**** Da rivedere ****/
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

int Server::get_max_fd(int sockfd)
{
	int max = sockfd;
	std::vector<Client *>::iterator i = clients.begin();
	while (i != clients.end())
	{
		max = max > (*i)->getFd() ? max : (*i)->getFd();
		i++;
	}
	return (max);
}

std::string Server::get_pass() const
{
	return (pass);
}

std::string Server::getDate() const
{
	return (time_string);
}

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
		quit_cmd(client, splitted);	//Estrapoliamo la reason direttamente in questa funzione
	else if(compStr(aStr, "INVITE"))
		invite_cmd(clientConvert(splitted), splitted[splitted.size() - 1], client);
	else if(compStr(aStr, "TOPIC"))
		topic_cmd(splitted[1], splitted, client);  //topicConvert non va bene, mandiamogli la splitted direttamente, poi in topic_cmd estrapoliamo il messaggio
	else if(compStr(aStr, "KICK"))
	{
		if (!splitted[3].empty())
			kick_cmd(splitted[1], splitted[2], client, splitted[3]);
		else
			kick_cmd(splitted[1], splitted[2], client, NULL);
	}
	else if(compStr(aStr, "JOIN"))
		join_cmd(client, splitted[1], splitted[2]);
	else if(compStr(aStr, "WHO"))
		who_cmd(splitted[1], client);
	else if(compStr(aStr, "WHOIS"))
		whois_cmd(splitted[1], client);
	else if(compStr(aStr, "PRIVMSG"))
		privmsg_cmd(client, splitted[1], splitted);
	else if(compStr(aStr, "MODE"))
		mode_cmd(client, splitted);
	else if(compStr(aStr, "LEAVE"))
		leave_cmd(client, channelConvert(splitted));
	else if(compStr(aStr, "PASS"))
		pass_cmd(client, splitted[1]);
	else
	{
		aStr.clear();
		aStr += splitted[0] + " is an unkown server command";
		send(client->getFd(), aStr.c_str(), aStr.length(), 0);
	}
	return (false);
}

void Server::quit_cmd(Client *client, std::vector<std::string> words)	/*****  Da rivedere  *****/
{
	//quitta dal server e puoi mandare un messaggio (no fucking flags)
	//int id = client->getFd();
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
		int it = 0;
		for (std::vector<Client *>::iterator l = channel_clients.begin(); l != channel_clients.end(); l++)
		{
			if (client->getNick() == channel_clients[it++]->getNick())
			{
				//fa parte di questo canale
				msg += client->getUser() + " " + client->getHost() + " has quit IRC: Quit: " + msg_quit;
				send_all(msg, *client);
				i->kickCmd(client, "quit");
			}
		}
		i++;
	}
	msg.clear();
	msg += "Server ERROR: :Closing Link: host " + client->getHost() + " " + "(Quit: " + msg_quit + ")";
	std::cout << msg << std::endl;
	//remove client from clients client_map
	msg.clear();
	client_map.erase(fd);
	clients.erase(findIterClient(getClient(fd)));
	close(fd);
	exit(0);
}

void Server::privmsg_cmd(Client *sender, std::string receiver, std::vector<std::string> mex)
{
	//controllare sia sul server
	// if(receiver != sender && FD_ISSET(receiver->getFd(), &write_fds))	//
	// {																	//
	// 	if((receiver->getFd(), mex.c_str(), mex.length(), 0) < 0)			//???
	// 		fatal();														//
	// }																	//

	std::string msg;
	std::vector<std::string>::iterator msgIt;
	
	if(receiver[0] == '#')
	{
		Channel *channel = getChannel(receiver);
		std::vector<Client *> clients = channel->getClients();
		std::vector<Client *>::iterator iter;

		if(channel->isOp(sender))
			msg += "<@" + sender->getNick() + ">: ";
		else if(channel->isHalfOp(sender))
			msg += "<%" + sender->getNick() + ">: ";
		else
			msg += "<" + sender->getNick() + ">: ";
		for(msgIt = mex.begin() + 2; msgIt != mex.end(); msgIt++)
		{
			msg += *msgIt;
			if (msgIt != mex.end() - 1)
				msg += " ";
		}
		int j = 0;
		for(iter = clients.begin(); iter != clients.end(); iter++)
			send(clients[j++]->getFd(), msg.c_str(), msg.length(), 0);
	}
	else //manda ad un utente //controllare che chi manda il messaggio sia autorizzato a mandarlo (ban, voice)
	{
		int fd = -1;
		int j = 0;
		for(std::vector<Client *>::iterator i = clients.begin(); i < clients.end(); i++)
			if(receiver == clients.at(j++)->getNick())
				fd = clients.at(j)->getFd();
		if(fd == -1)
		{
			msg += "[PRIVMSG > > > " + receiver + "]: ";
			for(msgIt = mex.begin() + 2; msgIt != mex.end(); msgIt++)
			{
				msg += *msgIt;
				if (msgIt != mex.end() - 1)
					msg += " ";
			}
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
			msg.clear();
			msg += receiver + ": no such nick/channel";
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
			return ;
		}
		else
		{
			msg += "[PRIVMSG > > > " + receiver + "]: ";
			for(msgIt = mex.begin() + 2; msgIt != mex.end(); msgIt++)
			{
				msg += *msgIt;
				if (msgIt != mex.end() - 1)
					msg += " ";
			}
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
			msg.clear();
			Client *msgReceiver = getClient(fd);
			msg += "<" + sender->getNick() + "> ";
			for(msgIt = mex.begin() + 2; msgIt != mex.end(); msgIt++)
			{
				msg += *msgIt;
				if (msgIt != mex.end() - 1)
					msg += " ";
			}
			send(msgReceiver->getFd(), msg.c_str(), msg.length(), 0);
		}
	}
}

void Server::invite_cmd(std::vector<Client *> invited, std::string channel_name, Client *sender)
{
	//gli inviti possono essere una lista di nomi
	//in funzione di questo controllo se quei client sono nel server
	//PS: vengono invitati solo se presenti

	Channel *channel;
	std::string msg;
	std::map<int, Client*>::iterator it = client_map.begin();
	std::map<std::string, Channel*>::iterator iter = channel_map.begin();

	while(iter != channel_map.end())
	{
		if(channel_name == iter->first)
			break;
		if(iter == channel_map.end())
		{
			msg += "401 " + sender->getNick() + " " + invited[0]->getNick() + ": No such name/channel";
			send(sender->getFd(), msg.c_str(), msg.size(), 0);
			return ;
		}
		iter++;
	}
	channel = this->getChannel(channel_name);

	for (uint i = 0; i < invited.size(); i++)
	{
		while(it != client_map.end())
		{
			if (invited[i]->getNick() == it->second->getNick())
				break;
			it++;
			if (it == client_map.end())
			{
				msg += "401 " + sender->getNick() + " " + invited[0]->getNick() + ": No such name/channel";
				send(sender->getFd(), msg.c_str(), msg.size(), 0);
				return ;
			}
		}
		channel->invite(invited[i]);
	}
}

void Server::topic_cmd(std::string channel_name, std::vector<std::string> splitted, Client *sender)
{
	Channel *channel;

	//estrapoliamo il messaggio
	std::string topic = "";
	std::vector<std::string> tmp;
	tmp.assign(splitted.begin() + 2, splitted.end());
	topic  = topicConvert(tmp);

	channel = this->getChannel(channel_name);
	if (topic == "")
	{
		if (!channel->getTopic().empty())
			std::cout << "Channel topic is: " << channel->getTopic() << std::endl;
		else
			std::cout << "No channel topic is set" << std::endl;
	}
	else
	{
		if (channel->setTopic(topic))
			std::cout << sender->getUser() << "[" << sender->getHost() << "] changed the topic to :" << topic << std::endl;
	}
}

void Server::kick_cmd(std::string channel_name, std::string client_name, Client *sender, std::string reason = "")
{
	Channel *channel;

	channel = this->getChannel(channel_name);
	for (uint i = 0; i < clients.size(); i++)
		if (clients[i]->getNick() == client_name)
			channel->kickCmd(clients[i], reason);
	if (reason != "")
		std::cout << client_name << " was kicked from " << channel_name << " by " << sender->getUser() << " because " << reason << std::endl;
	else
		std::cout << client_name << " was kicked from " << channel_name << " by " << sender->getUser() << " for no reason"<< std::endl;
}

void Server::join_cmd(Client *client, std::string channel_name, std::string psw = "")
{
	//controlla che channel_name esiste nel map e accedi al second
	Channel *channel;
	
	channel = this->getChannel(channel_name);
	channel->connect(client, psw);
}

//clients and channels management by server
Client* Server::getClient(int sockfd)
{
	for(std::map<int, Client*>::iterator it = client_map.begin(); it != client_map.end(); it++)
	{
		if(sockfd == it->first)
			return (it->second);
	}
	return (NULL);
}

Channel* Server::getChannel(std::string nameCh)
{
	for(std::map<std::string, Channel*>::iterator it = channel_map.begin(); it != channel_map.end(); it++)
	{
		if(nameCh == it->first)
			return (it->second);
	}
	return (NULL);
}