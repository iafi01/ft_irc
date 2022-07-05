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

std::vector<Channel *> Server::channelConvert(std::vector<std::string> splitted) //non serve, viene passato un solo param diviso da < , >
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
		fatal("err: creation socket");
	if(bind(this->sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		fatal("err: bind socket");
	if(listen(this->sockfd, 0) < 0)
		fatal("err: listen socket");
}

void Server::start_server()
{
	std::string w;
	int valread = 0;
	char buffer[1025];
	int fd = -1;
	int new_sd = -1;
	while (1)
	{
		FD_ZERO(&read_fds);
		FD_SET(sockfd, &read_fds);
		max_fd = sockfd;
		std::map<int, Client*>::iterator cli = client_map.begin();
		while (cli != client_map.end())
		{
			fd = cli->first;
			if (fd > 0)
				FD_SET(fd, &read_fds);
			if (fd > max_fd)
				max_fd = fd;
			cli++;
		}
		if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0)
			fatal("err: select socket");
		if (FD_ISSET(sockfd, &read_fds))
		{
			if ((new_sd = accept(max_fd, (struct sockaddr *)&serveraddr, (socklen_t *)&addrlen)) < 0)
				fatal("err: accept client");
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_sd , inet_ntoa(serveraddr.sin_addr) , ntohs(serveraddr.sin_port));
			// if (setsockopt(new_sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			//  	fatal("err: socket options");
			w.append ("Welcome to IRC!\n");
			if ((send(new_sd, w.c_str(), w.length(), 0)) < 0)
				fatal("err: send welcome message");
		}
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (clients_fd[i] == 0)
			{
				Client *new_client = new Client;
				clients_fd[i] = new_sd;
				new_client->setFd(new_sd);
				client_map.insert(std::make_pair(clients_fd[i], new_client));
				break;
			}
		}
		//controllo nuove_connessioni/disconnessioni e comandi per ogni user
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			fd = clients_fd[i];
			if (FD_ISSET(fd, &read_fds)) //Non entra qui dentro, va in segfault
            {
                if ((valread = read(fd, buffer, 1024)) == 0) //Cntrl+C
					std::cout << "CNTRL+C" << std::endl;/*Creare funzione per gestire il cntrl+c (deve quittare)*/
                else
                {
                    buffer[valread] = '\0';
                    if (client_map.find(fd)->second->getLog() == false) //se é la prima connessione e non ha loggato
					{
						std::cout << "First time logging" << std::endl;/*
						if (parse_info(client_map.find(sd)->second, buffer, valread, client_map) == -1)
							// Stessa cosa del cntrl+c, deve quittare 
					*/}
					else
						std::cout << "Already logged" << std::endl;
                    	//parse_commands(client_map.find(sd)->second, buffer, valread, i);
                }
            }
		}
	}
	
}

void Server::fatal(std::string s)
{
	std::cout << s << std::endl;
	exit(1);
}

//public
Server::Server() : server_name("localhost"), now(time(0)), time_string(ctime(&now))
{

}

Server::~Server()
{

}

Server::Server(const int port, const std::string pass) : server_name("localhost"), now(time(0)), time_string(ctime(&now))
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

/*void Server::clientRegister(Client *client)
{
	std::string msgOne = "Welcome to IRC Server!\n" + this->time_string + "Please Insert your nickname: ";
	std::string msgTwo = "now insert your username: ";
	int new_fd = client->getFd();

	send(new_fd, msgOne.c_str(), msgOne.length(), 0);
	
	
	
}*/

/*void Server::accept_client()
{
	int new_fd;
	Client *newClient;
	if((new_fd = accept(this->sockfd, (struct sockaddr *)&serveraddr, (socklen_t *)&addrlen)) < 0)
		fatal();
	newClient = new Client(new_fd);
	//clientRegister(newClient);
	//sprintf(this->server_buffer, "server: client %d just arrived\n", new_fd);
	// send_all(this->server_buffer, *getClient(new_fd)); //segfault
	// FD_SET(new_fd, &this->curr_fds);
	// client_map.insert(std::make_pair(new_fd, newClient));
	// clients.push_back(newClient);
}*/

void Server::send_all(std::string mex, Client sender)		/**** Da rivedere ****/
{
	int i = 0;
	while(clients[i])
	{
		if(clients[i] != &sender && FD_ISSET(clients[i]->getFd(), &write_fds))
		{
			if(send(clients[i]->getFd(), mex.c_str(), mex.length(), 0) < 0)
				fatal("err: send error on send_all funtion");
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
		who_cmd(splitted[1]);
	else if(compStr(aStr, "WHOIS"))
		/*whois_cmd(splitted, client)*/;
	else if(compStr(aStr, "PRIVMSG"))
		privmsg_cmd(client, splitted[1], splitted);
	else if(compStr(aStr, "MODE"))
		mode_cmd(client, splitted);
	else if(compStr(aStr, "PART"))
		part_cmd(client, splitted);
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

/*void Server::pass_cmd(Client *admin, std::string pass)
{
	
}*/

void Server::part_cmd(Client *client, std::vector<std::string> splitted)
{
	Channel *channel;
	std::string	msg;
	std::vector<std::string> names;

	names = ft_split(splitted[1], ",");
	if (splitted.size() == 1)
	{
		msg.append(": 461 " + client->getNick() + " PART :Not enough parameters\n");
		send(client->getFd(), msg.c_str(), msg.length(), 0);
	}
	else if (splitted.size() == 2)
	{
		for (uint i = 0; i < names.size(); i++)
		{
			channel = getChannel(names[i]);
			if (!channel)
			{
				msg.append(": 403 " + client->getNick() + " " + names[i] + " :No such channel\n");
				send(client->getFd(), msg.c_str(), msg.length(), 0);
			}
			else
			{
				if (!channel->isClient(client))
				{
					msg.append(" :442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
					send(client->getFd(), msg.c_str(), msg.length(), 0);
				}
				else
				{
					msg.append(":" + client->getNick() + "!~" + client->getUser() + " PART " + channel->getName() + "\n");
					send_all(msg, *client);
					send(client->getFd(), msg.c_str(), msg.length(), 0);
					channel->removeClient(client);
					if (channel->getClients().empty()) //se esce l'ultimo utente il canale viene eliminato
					{
						std::map<std::string, Channel*>::iterator i;

						i = channel_map.find(channel->getName());
						channel_map.erase(i);
					}
				}
			}
		}
	}
}

void Server::who_cmd(std::string filter)
{
	Channel *channel = NULL;
	std::vector <Client *>channel_clients;
	Client *user = NULL;

	if (filter[0] == '#')
	{
		channel = getChannel(filter);
		if (channel == NULL) //se il channel non esiste
		{
			std::cout << "Error Channel does not exist" << std::endl;
			return ;
		}
		channel_clients = channel->getClients();
		for (std::vector<Client *>::iterator i = channel_clients.begin(); i != channel_clients.end(); i++)
		{
			std::cout << "WHO entry for " << (*i)->getUser() << " [" << (*i)->getHost() << "]: Channel: " << channel->getName() << ", Server: " << this->server_name << std::endl;
			std::cout << "End of WHO list for " << channel->getName() << std::endl;
		}
	}
	else if (filter[0] != '#')
	{
		//cerco il client con filter come username
		for(std::map<int, Client*>::iterator it = client_map.begin(); it != client_map.end(); it++)
		{
			if(filter == it->second->getUser())
			{
				user = it->second;
				break;
			}
		}
		std::cout << "WHO entry for " << user->getUser() << " [" << user->getHost() << "]: Channel: " << channel->getName() << ", Server: " << this->server_name << std::endl;
	}
}

/*void Server::whois_cmd(std::vector<std::string> splitted, Client *sender)
{
	std::string msg;
	Client *infoUser;
	std::vector<Channel *> channelList;
	int userFd = -1;
	std::map<int, Client *>::iterator iter;
	std::map<int, Channel *>::iterator cIter;

	if(splitted.size() == 2)
	{
		for(iter = client_map.begin(); iter != client_map.end(); iter++)
			if(splitted[1] == iter->second->getNick() && splitted[1] != "localhost")
				userFd = iter->second->getFd();
		if(userFd != -1)
		{
			infoUser = getClient(userFd);
			for(cIter = channel_map.begin(); cIter != channel_map.end(); cIter++)
				if(cIter->second->isClient(infoUser))
					channelList.push_back(cIter->second);
			msg += infoUser->getNick() + "'s username is: " + infoUser->getUser() + "\n";
			if(!channelList.empty())
			{
				msg += infoUser->getNick() + "'s channel/s: ";
				for(std::vector<Channel *>::iterator lIter = channelList.end(); lIter != channelList.end(); lIter++)
				{
					if(lIter == channelList.end() - 1)
						msg += *lIter;
					else
						msg += *lIter + ", ";
				}
			}
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
		}
		else
		{
			msg += "No such nick";
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
		}
	}
	else if(splitted.size() == 3)
	{
		
	}
	else
	{
		msg += "";
	}
}*/

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