#include "../includes/Server.hpp"
#include <cstdlib>

int parameters = 0;

std::string Server::printTime()
{
	time_t rawtime;
    struct tm * timeinfo;
    char buffer [12];

	time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (buffer,12,"[%X] ",timeinfo);
    return (buffer);
}

Client *Server::getClientFromUser(std::string username, std::vector<Client *> clie)
{
	for (std::vector<Client*>::iterator i = clie.begin(); i != clie.end(); i++)
	{
		if ((*i)->getUser() == username)
		{
			return (*i);
		}
	}
	return (NULL);
}

int Server::findIterClient(Client *client)
{
	int j = 0;
	std::vector<Client*>::iterator i;

    i = clients.begin();
    while (i != clients.end())
    {
        if ((*i) == client)
        {   
            return (j);
        }
        i++;
		j++;
    }
    return (j);
}

std::vector<std::string> ft_split(std::string toSplit, std::string toFind)
{
	std::vector<std::string> splitted;
	std::string str;
	int i = 0;
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
	
	while(splitted[splitted.size() - 1][i] != '\0')
	{
		str += splitted[splitted.size() - 1][i];
		i++;
	}
	splitted.pop_back();
	splitted.push_back(str);
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
		msg += printTime() + channel_name + ": 482 You are not channel operator\n";
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
		msg += printTime() + channel_name + ": 482 You are not channel operator\n";
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
		msg += printTime() + channel_name + ": 482 You are not channel operator\n";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	for (uint i = 0; i < clientToHalfOp.size(); i++)
	{
		channel->halfOp(clientToHalfOp[i]);
	}
}

void Server::dehalf_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToDeHalfOp)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	if (!channel->isOp(admin))
	{
		msg += printTime() + channel_name + ": 482 You are not channel operator\n";
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
		msg += printTime() + channel_name + ": 482 You are not channel operator\n";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
}

void Server::unvoice_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToUnVoice)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	std::cout << "Entro in unvoice" << std::endl;
	if (channel->isOp(admin) || !channel->isHalfOp(admin))
	{
		std::cout << "E' admin" << std::endl;
		for (uint i = 0; i < clientToUnVoice.size(); i++)
		{
			std::cout << "Provo a fare il devoice" << std::endl;
			channel->deVoiceOp(clientToUnVoice[i]);
			std::cout << "Fatto il devoice" << std::endl;
		}
	}
	else
	{
		msg += printTime() + channel_name + ": 482 You are not channel operator\n";
		send(admin->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	return ;
}


void Server::ban_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToBan, std::string reason)
{
	Channel *channel = this->getChannel(channel_name);
	std::string msg;

	if (!channel->isOp(admin))
	{
		msg += printTime() + channel_name + ": You are not channel operator\n";
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
		msg += printTime() + channel_name + ": 482 You are not channel operator\n";
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

bool Server::compNames(std::string receiver, std::string nickname)
{
	int i = 0;
	while(nickname[i] != '\0')
	{
		if(nickname[i] != receiver[i])
			return false;
		i++;
	}
	if(nickname[i] == receiver[i])
		return true;
	return false;
}

//se i nick passati sono nel server, crea un nuovo vettore di clients
std::vector<Client *> Server::clientConvert(std::vector<std::string> splitted)
{
	std::vector<Client *> new_clients;
	for (uint j = 0; j < clients.size(); j++)
		for (uint i = 3; i < splitted.size(); i++)
			if (splitted[i] == clients[j]->getNick())
				new_clients.push_back(clients[j]);
	return (new_clients);
}

std::string Server::topicConvert(std::vector<std::string> toConv)
{
	std::string result;
	for (uint i = 0; i < toConv.size(); i++)
	{
		result += toConv[i];
		if (i == toConv.size() - 1) //se ?? l'ultima stringa prima del nome del channel
			result += "\0";
		else
			result +=  " ";
	}	
	return (result);
}

void Server::sendWelcome(Client *client)
{
	std::string msgWelcome;
	msgWelcome.append("001 " + client->getNick() + " :Welcome to the IRC Network, " + client->getUser() + "\n");
    msgWelcome.append("002 " + client->getNick() + " :Your host is IRC, running version 2.1" + "\n");
	msgWelcome.append("003 " + client->getNick() + " :This server was created " + printTime() + "\n");
	msgWelcome.append(": 372 :COMMANDS YOU CAN USE:\n\
: 372 :JOIN 	-> join or create channels\n\
: 372 :LIST	 	-> list of channels\n\
: 372 :PART 	-> leave a channel\n\
: 372 :PRIVMSG 	-> send a message to someone or to a channel\n\
: 372 :TOPIC	-> change topic of a channel\n\
: 372 :WHO		-> list of people in a channel\n\
: 372 :KICK		-> kick a user from a channel (OP/HALFOP ONLY)\n\
: 372 :MODE	COMMAND FLAGS:\n\
: 372 :+o/-o	-> op/deop a user in a channel\n\
: 372 :+h/-h	-> halfop/dehalfop a user in a channel\n\
: 372 :+v/-v	-> voice/devoice a user in a channel\n\
: 372 :+b/-b	-> ban/unban a nickname in a channel OR print a banlist if no user is given\n");
    send(client->getFd(), msgWelcome.c_str(), msgWelcome.length(), 0);
}

bool Server::check_nick(Client *new_client, char *buffer, int valread)
{
	std::vector<std::string> splitted;
	std::string	strings(buffer, (size_t)valread);
	std::string msg;

	splitted = ft_split(strings, "\n"); //CRLF fine cmd
	splitted.resize(1);
	new_client->setNick(splitted[0]);
	std::map<int, Client *>::iterator iter = client_map.begin();
	std::map<int, Client *>::iterator clientPos = client_map.begin();
	for (; clientPos != client_map.end(); clientPos++)
		if(clientPos->first == new_client->getFd())
			break;	
	for( ; iter != client_map.end(); iter++)
	{
		if (std::distance(client_map.begin(), iter) < std::distance(client_map.begin(), clientPos))
		{
			if(iter->second->getNick() == clientPos->second->getNick())
				clientPos->second->setNick(clientPos->second->getNick() + "|2");
		}
		else if(std::distance(client_map.begin(), iter) > std::distance(client_map.begin(), clientPos))
		{
			if(iter->second->getNick() == clientPos->second->getNick())
				iter->second->setNick(iter->second->getNick() + "|2");
		}
	}
	msg.append(printTime() + "Now insert your username: ");
	send(new_client->getFd(), msg.c_str(), msg.length(), 0);
	return (true);
}

bool Server::check_user(Client *new_client, char *buffer, int valread)
{
	std::vector<std::string> splitted;
	std::string	strings(buffer, (size_t)valread);
	std::string msg;

	splitted = ft_split(strings, "\n"); //CRLF fine cmd
	splitted.resize(1);
	new_client->setUser(splitted[0]);
	std::map<int, Client *>::iterator iter = client_map.begin();
	std::map<int, Client *>::iterator clientPos = client_map.begin();
	for (; clientPos != client_map.end(); clientPos++)
		if(clientPos->first == new_client->getFd())
			break;	
	for( ; iter != client_map.end(); iter++)
	{
		if (std::distance(client_map.begin(), iter) < std::distance(client_map.begin(), clientPos))
		{
			if(iter->second->getUser() == clientPos->second->getUser())
				clientPos->second->setUser(clientPos->second->getUser() + "|2");
		}
		else if(std::distance(client_map.begin(), iter) > std::distance(client_map.begin(), clientPos))
		{
			if(iter->second->getUser() == clientPos->second->getUser())
				iter->second->setUser(iter->second->getUser() + "|2");
		}
	}
	msg.append(printTime());
	send(new_client->getFd(), msg.c_str(), msg.length(), 0);
	sendWelcome(new_client);
	return (true);
}

bool	Server::check_pass(Client *new_client, char *buffer, int valread)
{
	std::vector<std::string> splitted;
	std::string	strings(buffer, (size_t)valread);
	std::vector<std::string> user;
	std::string msg;

	splitted = ft_split(strings, "\r\n"); //CRLF fine cmd
	splitted.resize(1);
	if (!splitted[0].compare(0, 6,"PASS :")) //irc_client
	{
		//set pass
		splitted[0] = splitted[0].substr(6, splitted[0].length() - 1);
		//set nick
		splitted[1] = splitted[1].substr(5, splitted[1].length() - 1);
		//set user
		user = ft_split(splitted[2], " ");

		new_client->setNick(splitted[1]);
		new_client->setUser(user[1]);
		this->irc_client = 1;
		new_client->setIsLogged(true);
		//setted nick, user and control irc_client == 0 in check_user,check_nick
	}
	if (irc_client == 0)
	{
		splitted = ft_split(strings, "\n");
		splitted.resize(1);
		msg.append(printTime() + "Now insert your nickname: ");
		send(new_client->getFd(), msg.c_str(), msg.length(), 0);
		new_client->setIsLogged(true);
		return 1;
	}
	if (this->pass != splitted[0])
	{
		msg.append(printTime() + ": 464 Error: Password incorrect\n" + printTime() + "Please insert the password: ");
		send(new_client->getFd(), msg.c_str(), msg.length(), 0);
		return (-1);
	}
	sendWelcome(new_client);
	msg.clear();
	return (1);
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
	irc_client = 0;
	pass += "\0";

	this->sockfd = -1;
	opt = 1;
	if((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		fatal("Error: creation socket");
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )
	{
		perror("Error :setsockopt failed");
		exit(EXIT_FAILURE);
	}
	if(bind(this->sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		fatal("Error: bind socket");
	if(listen(this->sockfd, 0) < 0)
		fatal("Error: listen socket");
}

void Server::start_server()
{
	int valread = 0;
	char buffer[1025];
	int fd = -1;
	int new_fd = -1;
	int opt = 1;
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
			perror("Error: select socket");
		if (FD_ISSET(sockfd, &read_fds))
		{
			if ((new_fd = accept(sockfd, (struct sockaddr *)&serveraddr, (socklen_t *)&addrlen)) < 0)
				fatal("Error: accept client");
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_fd , inet_ntoa(serveraddr.sin_addr) , ntohs(serveraddr.sin_port));
			if (setsockopt(new_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			{
			 	perror("Error: setsockopt failed");
				exit(1);
			}
			std::string w;
			w.append (printTime() + "Welcome! Please insert the password: ");
			if ((send(new_fd, w.c_str(), w.length(), 0)) < 0)
				perror("Error: send welcome message");
			Client *new_client = new Client;
			new_client->setFd(new_fd);
			clients.push_back(new_client);
			client_map.insert(std::make_pair(new_fd, new_client));
		}
		std::vector<std::string> tmp_void;
		for (std::vector<Client *>::iterator i = clients.begin(); i != clients.end(); i++)
        {
            fd = (*i)->getFd();
            if (FD_ISSET(fd, &read_fds))
            {
                bzero(buffer, sizeof(buffer));
                if ((valread = read(fd, buffer, 1024)) == 0)
                {
                    continue ;
                }
                else if (buffer[0] == 0 || buffer[0] == 3 || buffer[0] == '\n')
                {
                    std::string err = printTime () + "Error: inserted\n";
                    if (parameters == 0)
                        err += printTime() + "Please insert a valid password: ";
                    else if (parameters > 0)
                        err += printTime() + "Please insert a valid nickname: ";
                    else
                        err += printTime() + "Please insert a valid username: ";
                    send((*i)->getFd(), err.c_str(), err.length(), 0);
                    continue ;
                }
                else
                {
                    buffer[valread] = '\0';
                    Client *cli = client_map.find(fd)->second;
                    if (cli->getIsLogged() == false)
                    {
                        if (check_pass(*i, buffer, valread) == false)
                            exit(1);
                    }
                    else if (cli->getNick().empty() && cli->getIsLogged() == true)
                    {
                        parameters = -1;
                        if (check_nick(*i, buffer, valread) == false)
                            exit(1);
                    }
                    else if (cli->getUser().empty() && !cli->getNick().empty())
                    {
                        if (check_user(*i, buffer, valread) == false)
                            exit(1);
                    }
                    else if (cli->getIsLogged() == true && !cli->getNick().empty() && !cli->getUser().empty())
                    {
                        buffer[valread - 1] = '\0';
                        if (parse_commands(*i, buffer, valread))
                        {
                            if (clients.empty())
                                break;
                            i = clients.begin();
                            continue ;
                        }
                        std::string msg = printTime();
                        send(cli->getFd(), msg.c_str(), msg.length(), 0);
                    }
                }
            }
        }
    }
}



void Server::fatal(std::string s)
{
	std::perror(s.c_str());
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
				fatal("Error: send error on send_all funtion");
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
	if(compStr(aStr, "QUIT") || compStr(aStr, "/QUIT"))
		return (quit_cmd(client, splitted));	//Estrapoliamo la reason direttamente in questa funzione
	else if(compStr(aStr, "INVITE") || compStr(aStr, "/INVITE"))
		invite_cmd(clientConvert(splitted), splitted[splitted.size() - 1], client);
	else if(compStr(aStr, "TOPIC") || compStr(aStr, "/TOPIC"))
		topic_cmd(splitted[1], splitted, client);  //topicConvert non va bene, mandiamogli la splitted direttamente, poi in topic_cmd estrapoliamo il messaggio
	else if(compStr(aStr, "KICK") || compStr(aStr, "/KICK"))
	{
		if (!splitted[3].empty())
			kick_cmd(splitted[1], splitted[2], client, splitted[3]);
		else
			kick_cmd(splitted[1], splitted[2], client, "");
	}
	else if(compStr(aStr, "JOIN") || compStr(aStr, "/JOIN"))
		join_cmd(client, splitted[1], splitted[2]);
	else if(compStr(aStr, "WHO") || compStr(aStr, "/WHO"))
		who_cmd(splitted[1], client);
	else if(compStr(aStr, "PRIVMSG") || compStr(aStr, "/PRIVMSG"))
		privmsg_cmd(client, splitted[1], splitted);
	else if(compStr(aStr, "MODE") || compStr(aStr, "/MODE"))
		mode_cmd(client, splitted);
	else if(compStr(aStr, "PART") || compStr(aStr, "/PART"))
		part_cmd(client, splitted);
	else
	{
		aStr.clear();
		aStr += printTime() + splitted[0] + " is an unkown server command\n";
		send(client->getFd(), aStr.c_str(), aStr.length(), 0);
	}
	return (false);
}

void Server::notQuitCmd(int fd, int i)
{
	Client *clientToErase = getClient(fd);
	std::string msg;
	
	getsockname(fd , (struct sockaddr*)&serveraddr , (socklen_t*)&addrlen);
	std::cout << "The disconnected host was named " << clients[i]->getUser() << std::endl;
	msg.append(printTime() + ":" + clientToErase->getNick() + "!" + clientToErase->getUser() + ": QUIT " + "\n");
	if (!channel_map.empty())
	{
		for(std::map<std::string, Channel*>::iterator it = channel_map.begin(); it != channel_map.end(); it++)
		{
			if(clientToErase != NULL)
			{
				
				for (std::vector<Client *>::iterator c = (*it).second->getClients().begin(); c != (*it).second->getClients().end(); c++)
					send((*c)->getFd(), msg.c_str(), msg.length(), 0);
				(*it).second->disconnect(clientToErase);
			}
		}
	}
	clients[i]->setIsLogged(false);
	for (std::vector<Client *>::iterator j = clients.begin(); j != clients.end(); j++)
	{
		if ((*j) == clients[i])
		{
			clients.erase(j);
			break;
		}
	}
	client_map.erase(fd);
	close(fd);
}

bool Server::quit_cmd(Client *client, std::vector<std::string> words)	/*****  Da rivedere  *****/
{
	int fd = client->getFd();
	int id = findIterClient(client);
	if (words.size() == 1)
	{
		notQuitCmd(fd, id);
		return (true);
	}
	getsockname(fd , (struct sockaddr*)&serveraddr , (socklen_t*)&addrlen);
	std::cout << "The disconnected host was named " << client->getUser() << std::endl;
	std::string msg;
	int y = 1;
	std::string quitmsg;
	while (y < (int)words.size())
	{
		quitmsg.append(words[y]);
		quitmsg.append(" ");
		y++;
	}
	msg.append(printTime() + ":" + client->getNick() + "!" + client->getUser() + " QUIT: " + quitmsg + "\n");
	if (!channel_map.empty())
	{
		for(std::map<std::string, Channel*>::iterator it = channel_map.begin(); it != channel_map.end(); it++)
		{
			if(client != NULL)
			{
				
				for (std::vector<Client *>::iterator c = (*it).second->getClients().begin(); c != (*it).second->getClients().end(); c++)
					send((*c)->getFd(), msg.c_str(), msg.length(), 0);
				(*it).second->disconnect(client);
			}
		}
	}
	msg.clear();
	msg.append(printTime() + "ERROR: :Closing Link: " + client->getNick() + " (Quit: " + client->getUser() + ")\n");
	send(fd, msg.c_str(), msg.length(), 0);
	client[id].setIsLogged(false);
	client_map.erase(fd);
	close(fd);
	int k = 0;
	for (std::vector<Client *>::iterator j = clients.begin(); j != clients.end(); j++)
	{
		if (k++ == id)
		{
			clients.erase(j);
			return (true);
		}
	}
	return (false);
}

void Server::privmsg_cmd(Client *sender, std::string receiver, std::vector<std::string> mex)
{
	std::string msg;
	std::vector<std::string>::iterator msgIt;

	if(receiver[0] == '#')//controllare che chi manda il messaggio sia autorizzato a mandarlo (ban, voice)
	{
		if(getChannel(receiver) == NULL)
		{
			msg += printTime() + receiver + ": no such nick/channel\n";
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
			return ;
		}
		Channel *channel = getChannel(receiver);
		std::vector<Client *> clients = channel->getClients();
		//std::cout << channel->getName() << std::endl;
		if (channel->isBanned(sender))
		{
			msg += sender->getUser() + ": you are banned from the channel\n";
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
			return ;
		}
		std::vector<Client *>::iterator iter;
		
		if(channel->isOp(sender))
			msg += printTime() + channel->getName() + " <@" + sender->getNick() + ">: ";
		else if(channel->isHalfOp(sender) && channel->isVoiceOp(sender))
			msg += printTime() + channel->getName() +  " <%" + sender->getNick() + ">: ";
		else if (channel->isClient(sender) && channel->isVoiceOp(sender))
			msg += printTime() + channel->getName() +  " <" + sender->getNick() + ">: ";
		else if(!channel->isVoiceOp(sender) && channel->isClient(sender))
		{
			std::string err = printTime() + "You can not write on this channel\n";
			send(sender->getFd(), err.c_str(), err.length(), 0);
			return;
		}	
		else
		{
			std::string err = printTime() + "You are not on this channel\n";
			send(sender->getFd(), err.c_str(), err.length(), 0);
			return;
		}
		for(msgIt = mex.begin() + 2; msgIt != mex.end(); msgIt++)
		{
			msg += *msgIt;
			if (msgIt != mex.end() - 1)
				msg += " ";
		}
		int j = 0;
		msg += "\n";
		for(iter = clients.begin(); iter != clients.end(); iter++)
			send(clients[j++]->getFd(), msg.c_str(), msg.length(), 0);
	}
	else //manda ad un utente 
	{
		int fd = -1;
		std::string test;
		for(std::vector<Client *>::iterator i = clients.begin(); i != clients.end(); i++)
		{
			if(receiver == (*i)->getNick())
			{
				fd = (*i)->getFd();
				break;
			}
		}
		if(fd == -1)
		{
			msg += printTime() + "[PRIVMSG > > > " + receiver + "]: ";
			for(msgIt = mex.begin() + 2; msgIt != mex.end(); msgIt++)
			{
				msg += *msgIt;
				if (msgIt != mex.end() - 1)
					msg += " ";
			}
			msg += "\n";
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
			msg.clear();
			msg += printTime() + receiver + ": no such nick/channel\n";
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
			return ;
		}
		else
		{
			msg += printTime() + "[PRIVMSG > > > " + receiver + "]: ";
			for(msgIt = mex.begin() + 2; msgIt != mex.end(); msgIt++)
			{
				msg += *msgIt;
				if (msgIt != mex.end() - 1)
					msg += " ";
			}
			msg += "\n";
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
			msg.clear();
			Client *msgReceiver = getClient(fd);
			msg += printTime() + "<" + sender->getNick() + "> ";
			for(msgIt = mex.begin() + 2; msgIt != mex.end(); msgIt++)
			{
				msg += *msgIt;
				if (msgIt != mex.end() - 1)
					msg += " ";
			}
			msg += "\n";
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
			msg += printTime() + "401 " + sender->getNick() + " " + invited[0]->getNick() + ": No such name/channel";
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
				msg += printTime() + "401 " + sender->getNick() + " " + invited[0]->getNick() + ": No such name/channel";
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
	//send al posto di cout

	//estrapoliamo il messaggio
	std::string msg;
	std::string topic = "";
	std::vector<std::string> tmp;
	tmp.assign(splitted.begin() + 2, splitted.end());
	topic  = topicConvert(tmp);

	channel = this->getChannel(channel_name);
	if (!channel)
	{
		std::string err = printTime() + "No channel name found\n";
		send(sender->getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	if (topic == "")
	{
		if (!channel->getTopic().empty())
		{
			msg = printTime() + "Channel topic is: ";
			msg.append(channel->getTopic());
			msg.append("\n");
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
		}
		else
		{
			msg = printTime() + "No channel topic is set\n";
			send(sender->getFd(), msg.c_str(), msg.length(), 0);
		}
	}
	else
	{
		//controllo se sender ?? admin
		if (channel->setTopic(topic))
		{
			msg = printTime();
			if (!channel->isOp(sender))
			{
				msg += "You are not Op\n";
				send(sender->getFd(), msg.c_str(), msg.length(), 0);
				return ;
			}
			msg = sender->getUser();
			msg.append(" changed the topic to: ");
			msg.append(topic);
			msg.append("\n");
			for(std::vector<Client *>::iterator iter = channel->getClients().begin(); iter != channel->getClients().end(); iter++)
			{
				send((*iter)->getFd(), msg.c_str(), msg.length(), 0);
			}
		}
	}
}

void Server::kick_cmd(std::string channel_name, std::string client_name, Client *sender, std::string reason = "")
{
	Channel *channel = NULL;
	Client *kicked = NULL;
	std::string msg; //controllo admin e msg al channel

	if (this->getChannel(channel_name) == NULL)
	{
		std::string err = printTime() + "Error: channel does not exists\n";
		send(sender->getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	channel = this->getChannel(channel_name);
	if (!channel->isOp(sender))
	{
		msg = printTime() + "You are not an operator\n";
		send(sender->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	//ottenere il client dal user
	kicked = getClientFromUser(client_name, channel->getClients());
	//sta cazz e funzion ritorna NULL
	if (kicked == NULL || !channel->isClient(kicked))
	{
		msg = printTime() + "The client is not in the channel\n";
		send(sender->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	//sas
	if (reason != "")
	{
		std::cout << printTime();
		msg = printTime() + client_name;
		msg.append(" was kicked from ");
		msg.append(channel_name);
		msg.append(" by ");
		msg.append(sender->getUser());
		msg.append(" because ");
		msg.append(reason);
		msg.append("\n");
	}
	else
	{
		std::cout << printTime();
		msg = printTime() + client_name;
		msg.append(" was kicked from ");
		msg.append(channel_name);
		msg.append(" by ");
		msg.append(sender->getUser());
		msg.append(" for no reason");
		msg.append("\n");
	}
	for (std::vector<Client *>::iterator i = channel->getClients().begin(); i != channel->getClients().end(); i++)
		send((*i)->getFd(), msg.c_str(), msg.length(), 0);
	for (uint i = 0; i < clients.size(); i++)
	{
		msg.clear();
		if (clients[i]->getNick() == client_name)
		{
			channel->kickCmd(clients[i], reason);
			msg = printTime() + "Please re-join the channel\n";
			send(clients[i]->getFd(), msg.c_str(), msg.length(), 0);
		}
	}
}

void Server::join_cmd(Client *client, std::string channel_name, std::string psw = "")
{
	//controlla che channel_name esiste nel map e accedi al second
	std::string err;
	std::string msg;
	if(this->getChannel(channel_name) == NULL)
	{
		if (channel_name[0] != '#')
		{
			err = "403 "  + client->getNick() + " " + channel_name + " :No such channel" + "\n";
			send(client->getFd(), err.c_str(), err.length(), 0);
			return ;
		}
		else
		{
			Channel *channel = new Channel(channel_name, 100, 0, psw, "");
			msg += ":" + client->getNick() + "!" + client->getUser() + "@127.0.0.1 JOIN " + channel_name + "\n";
			send(client->getFd(), msg.c_str(), msg.length(), 0);
			msg.clear();
			addChannel(channel);
			channel->op(client);
			channel->connect(client, psw);
			if(!channel->getTopic().empty())
			{
				msg += ": 332 " + client->getNick() + " " + channel_name + " :" + channel->getTopic() + "\n";
				send(client->getFd(), msg.c_str(), msg.length(), 0);
				msg.clear();
			}
			else
			{
				msg += ": 331 " + client->getNick() + channel_name + " :No topic is set" + "\n";
				send(client->getFd(), msg.c_str(), msg.length(), 0);
				msg.clear();
			}
			std::vector<Client *>vec = channel->getClients();
			msg += ":" + client->getNick() + "!" + client->getUser() + " JOIN :" + channel_name + "\n";
			for(std::vector<Client *>::iterator k = vec.begin(); k != vec.end(); k++)
			{
				if((*k)->getFd() != client->getFd())	
					send((*k)->getFd(), msg.c_str(), msg.length(), 0);
			}
			msg.clear();
			for(std::vector<Client *>::iterator k = vec.begin(); k != vec.end(); k++)
			{
				if(channel->isOp(*k))
				{
					msg += ":127.0.0.1 353 " + (*k)->getNick() + " = " + channel_name + " :@" + (*k)->getNick() + "\n";
					send(client->getFd(), msg.c_str(), msg.length(), 0);
					msg.clear();
				}
			}
			for(std::vector<Client *>::iterator k = vec.begin(); k != vec.end(); k++)
			{
				if(!channel->isOp(*k))
				{
					msg += ":127.0.0.1 353 " + (*k)->getNick() + " = " + channel_name + " :@" + (*k)->getNick() + "\n";
					send(client->getFd(), msg.c_str(), msg.length(), 0);
					msg.clear();
				}
			}
			msg += ": 366 " + client->getNick() + channel_name + " :End of NAMES list\n"; 
		}
	}
	else
	{
		Channel *channel = getChannel(channel_name);
		if (channel->isClient(client))
			return ;
		msg.append(":" + client->getNick() + "!" + client->getUser() + "@127.0.0.1 JOIN " + channel_name + "\n");
		send(client->getFd(), msg.c_str(), msg.length(), 0);
		msg.clear();
		msg.append(":127.0.0.1 353 " + client->getNick() + " = " + channel_name + " :@" + client->getNick() + "\n");
		send(client->getFd(), msg.c_str(), msg.length(), 0);
		msg.clear();
		msg.append(":127.0.0.1 366 " + client->getNick() + " " + channel_name + " :End of /NAMES list.\n331\n");
		send(client->getFd(), msg.c_str(), msg.length(), 0);
		channel->connect(client, psw);
		addChannel(channel);
	}
}

void Server::part_cmd(Client *client, std::vector<std::string> splitted)
{
	Channel *channel = NULL;
	std::string	msg;
	std::vector<std::string> names;

	names.push_back(splitted[1]);
	if (splitted.size() == 1)
	{
		msg.append(printTime() + ": 461 " + client->getNick() + " PART :Not enough parameters\n");
		send(client->getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}
	else if (splitted.size() == 2)
	{
		for (uint i = 0; i < names.size(); i++)
		{
			channel = getChannel(splitted[1]);
			if (!channel)
			{
				msg.append(": 403 " + client->getNick() + " " + splitted[1] + " :No such channel\n");
				send(client->getFd(), msg.c_str(), msg.length(), 0);
			}
			else
			{
				if (!channel->isClient(client))
				{
					msg.append(printTime() + ": 403 " + client->getNick() + " " + names[i] + " :No such channel\n");
					send(client->getFd(), msg.c_str(), msg.length(), 0);
				}
				else
				{
					if (!channel->isClient(client))
					{
						msg.append(printTime() + " :442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
						send(client->getFd(), msg.c_str(), msg.length(), 0);
					}
					else
					{
						msg.append(printTime() + ":" + client->getNick() + "!~" + client->getUser() + " PART " + channel->getName() + "\n");
						send_all(printTime() + msg, *client);
						send(client->getFd(), msg.c_str(), msg.length(), 0);
						channel->removeClient(client);
						channel->decrementClient();
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
}

void Server::who_cmd(std::string filter, Client *client)
{
	Channel *channel = NULL;
	std::vector <Client *>channel_clients;
	std::string msg;
	std::string msg2;

	if (filter[0] == '#')
	{
		channel = getChannel(filter);
		if (channel == NULL) //se il channel non esiste
		{
			msg += printTime() + "Error: Channel does not exist\n";
			send(client->getFd(), msg.c_str(), msg.length(), 0);
			return ;
		}
		channel_clients = channel->getClients();
		for (std::vector<Client *>::iterator i = channel_clients.begin(); i != channel_clients.end(); i++)
		{
			msg = printTime();
			msg += "WHO entry for " + (*i)->getUser() + " [" + (*i)->getHost() + "]: Channel: " + channel->getName() + ", Server: " + this->server_name + "\n";
			msg2 = printTime();
			msg2 += "End of WHO list for " + channel->getName() + "\n";
			send(client->getFd(), msg.c_str(), msg.length(), 0);
			send(client->getFd(), msg2.c_str(), msg2.length(), 0);
		}
	}
	else if (filter[0] != '#')
	{
		//cerco il client con filter come username
		for(std::map<int, Client*>::iterator it = client_map.begin(); it != client_map.end(); it++)
		{
			if(filter == it->second->getUser())
			{
				msg += printTime() + "WHO entry for " + it->second->getUser() + " [" + it->second->getHost() + "]: Server: " + this->server_name + "\n";
				send(client->getFd(), msg.c_str(), msg.length(), 0);
				break;
			}
		}
	}
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
		if(nameCh == it->first) //qui ?? il problema, lenght 5 su #ppp
			return (it->second);
	}
	return (NULL);
}

void Server::addChannel(Channel *toAdd)
{
	//std::cout << printTime() << "creo canale :" << toAdd->getName() << std::endl;
	this->channel_map.insert(std::make_pair(toAdd->getName(), toAdd));
}