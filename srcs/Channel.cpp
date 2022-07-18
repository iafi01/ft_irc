#include "../includes/Channel.hpp"

Channel::Channel()
{

}

Channel::~Channel()
{

}

Channel::Channel(std::string name, int userLimit, int is_only_invite, std::string psw = "", std::string topic = "")
{
    this->name = name;
    this->userLimit = userLimit;
    this->is_only_invite = is_only_invite;
    this->nClient = 0;
    this->setPass(psw);
    this->setTopic(topic);
}

bool Channel::setPass(std::string psw)
{
    try {
        pass = psw;
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

void Channel::addClient(Client *toAdd)
{
    clients.push_back(toAdd);
}

void Channel::removeClient(Client *client)
{
    for(std::vector<Client *>::iterator i = clients.begin(); i != clients.end(); i++)
    {
        if ((*i)->getNick() == client->getNick())
        {
            this->clients.erase(i);
            if(this->isOp(client))
                this->deop(client);
            if(this->isHalfOp(client))
                this->deHalfOp(client);
            if(this->isInvited(client))
                this->removeInvite(client);
            if(this->isVoiceOp(client))
                this->deVoiceOp(client);
            return ;
        }
    }
    return ;
}

bool Channel::op(Client *client)
{
    try {
        this->op_vec.push_back(client);
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

void Channel::deop(Client *client)
{
    std::vector<Client *>::iterator ite = op_vec.end();
    std::vector<Client*>::iterator i;

    i = op_vec.begin();
    for(i = op_vec.begin(); i != op_vec.end(); i++)
    {
        if ((*(*i)).getNick() == client->getNick())
        {
            ite = i;
            break;
        }
    }
    if(ite != op_vec.end())
    {
        op_vec.erase(ite);
        return ;
    }
    return ;
}

bool Channel::halfOp(Client *client)
{
    try {
        this->half_op_vec.push_back(client);
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

bool Channel::deHalfOp(Client *client)
{
    std::vector<Client *>::iterator ite = half_op_vec.end();
    std::vector<Client*>::iterator i;

    i = half_op_vec.begin();
    for(i = half_op_vec.begin(); i != half_op_vec.end(); i++)
    {
        if ((*(*i)).getNick() == client->getNick())
        {
            ite = i;
            break;
        }
    }
    if(ite != half_op_vec.end())
    {
        half_op_vec.erase(ite);
        return true;
    }
    return (false);
}

bool Channel::voiceOp(Client *client)
{
    try {
        this->voice_op_vec.push_back(client);
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

bool Channel::deVoiceOp(Client *client)
{
    std::vector<Client *>::iterator ite = voice_op_vec.end();
    std::vector<Client*>::iterator i;

    i = voice_op_vec.begin();
    for(i = voice_op_vec.begin(); i != voice_op_vec.end(); i++)
    {
        if ((*(*i)).getNick() == client->getNick())
        {
            ite = i;
            break;
        }
    }
    if(ite != voice_op_vec.end())
    {
        voice_op_vec.erase(ite);
        return true;
    }
    return (false);
}

bool Channel::ban(Client *admin, std::string nick = "*", std::string user = "*", std::string host = "*", std::string _reason = "")
{
    try {
        Banned victim;

        time_t now = time(0);

        victim.nick = nick;
        victim.user = user;
        victim.host = host;

        victim.ban_time = ctime(&now);
        victim.admin = admin;

        if (_reason == "")
            victim.reason = "no reason";
        else
            victim.reason = _reason;

        this->banned_vec.push_back(victim);
        std::cout << "MAAA!" << this->banned_vec[0].user << std::endl;
        //stampa
        std::string msg = admin->getUser();
        msg += " has banned mask " + victim.nick + "!" + victim.user + "@" + victim.host + " for: ";
        if (_reason == "")
            msg += "no reason";
        else
            msg += _reason;
        msg.append("\n");
        for (std::vector<Client *>::iterator i = clients.begin(); i != clients.end(); i++)
            send((*i)->getFd(), msg.c_str(), msg.length(), 0);
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

bool Channel::unBan(std::string nick = "*", std::string user = "*", std::string host = "*")
{
    std::vector<Banned>::iterator i;

    int j = 0;
    i = banned_vec.begin();
    while (i != banned_vec.end())
    {
        if (banned_vec.at(j).nick == nick && banned_vec.at(j).user == user && banned_vec.at(j).host == host)
        {
            try {
                this->banned_vec.erase(i);
                std::string msg = "unban mask ";
                msg += nick + "!" + user + "@" + host + "\n";
                for (std::vector<Client *>::iterator i = clients.begin(); i != clients.end(); i++)
                    send((*i)->getFd(), msg.c_str(), msg.length(), 0);
                return (true);
            }
            catch (std::exception& e) {
                std::cerr << e.what();
                return (false);
            }
        }
        i++;
        j++;
    }
    
    return (false);
}

bool Channel::modInvite(enum modInvite i)
{
    try{
        is_only_invite = i;
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
    return (false);
}


std::string Channel::getName() const
{
    return (name);
}

std::string Channel::getTopic() const
{
    return (topic);
}

std::vector<Client*> Channel::getClients() const
{
    return (clients);
}

std::vector<Client*> Channel::getOp() const
{
    return (op_vec);
}

std::vector<Client*> Channel::halfOp() const
{
    return (half_op_vec);
}

std::vector<Client*> Channel::voiceOp() const
{
    return (voice_op_vec);
}
std::vector<Channel::Banned> Channel::getBanned() const
{
    return (banned_vec);
}

bool Channel::setTopic(std::string topic)
{
    try {
        this->topic = topic;
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

//cmds
bool Channel::kickCmd(Client *client, std::string _reason = "")
{
    try {
        std::cout << _reason << std::endl;
        disconnect(client);
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

bool Channel::inviteCmd(Client *client)
{
    try {
        invited_vec.push_back(client);
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

bool Channel::topicCmd(std::string setTopic)
{
    try {
        topic = setTopic;
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

bool Channel::isClient(const Client* client)
{
    std::vector<Client*>::iterator i;

    i = clients.begin();
    while (i != clients.end())
    {
        if ((*i) == client)
            return (true);
        i++;
    }
    return (false);
}

bool Channel::isInvited(const Client* client)
{
    std::vector<Client*>::iterator i;

    i = invited_vec.begin();
    while (i != invited_vec.end())
    {
        if ((*i) == client)
            return (true);
        i++;
    }
    return (false);
}

bool Channel::isOp(const Client* client)
{
    for(std::vector<Client *>::iterator i = this->op_vec.begin(); i != op_vec.end(); i++)
    {
        if ((*i) == client)
            return (true);
    }
    return (false);
}

bool Channel::isHalfOp(const Client* client)
{
    std::vector<Client*>::iterator i;

    i = half_op_vec.begin();
    while (i != half_op_vec.end())
    {
        if ((*i) == client)
            return (true);
        i++;
    }
    return (false);
}

bool Channel::isVoiceOp(const Client* client)
{
    std::vector<Client*>::iterator i;

    i = voice_op_vec.begin();
    while (i != voice_op_vec.end())
    {
        if ((*i) == client)
            return (true);
        i++;
    }
    return (false);
}

bool Channel::isBanned(const Client* client)
{
    for (int i = 0; i < (int)banned_vec.size(); i++)
	{
        //queste stringhe contengono toxic characters
        //std::cout << this->banned_vec[i].nick << this->banned_vec[i].user << this->banned_vec[i].reason << " == " << client->getUser() << std::endl;
		if (this->banned_vec[i].user == client->getUser())
			return (true);
	}
	return (false);
}

bool Channel::invite( Client* client)
{
    try {
        this->invited_vec.push_back(client);
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

bool Channel::removeInvite(const Client* client)
{
    std::vector<Client*>::iterator i;

    i = clients.begin();
    while (i != clients.end())
    {
        if ((*i) == client)
        {   
            try {
                this->invited_vec.erase(i);
                return (true);
            }
            catch (std::exception& e) {
                std::cerr << e.what();
                return (false);
            }
        }
        i++;
    }
    return (false);
}

void Channel::connect(Client* client, std::string psw = "")
{
    std::vector<Banned>::iterator i;
    std::string err;
    if(!banned_vec.empty())
    {
        i = banned_vec.begin();
        //se sei bannato esce dalla funzione
        while (i != banned_vec.end())
        {
            if ((client->getNick() == (*i).nick || client->getNick() == "*") &&
                (client->getUser() == (*i).user || client->getUser() == "*") &&
                (client->getHost() == (*i).host || client->getHost() == "*"))
                return ;
            i++;
        }
    }
    if (pass != psw)
    {
        if (pass != psw + "")
        {
            err = "pass of the channel incorrect\n";
            send(client->getFd(), err.c_str(), err.length(), 0);
            return ;
        }
    }
    if (nClient + 1 > userLimit)
    {

			std::cout << "channel is full" << nClient << "/" << userLimit << "\n";
            err = "channel is full!\n";
			send(client->getFd(), err.c_str(), err.length(), 0);
			return ;
		}
    if (is_only_invite > 0)
    {
        std::vector<Client*>::iterator j;
        bool invited;

        invited = false;
        j = invited_vec.begin();
        while (j != invited_vec.end())
        {
            if ((*j)->getUser() == client->getUser() && (*j)->getNick() == client->getNick() && (*j)->getHost() == client->getHost())
            {
                invited = true;
                break;
            }
            j++;
        }
        if (!invited)
        {
			err = "channel is only invite\n";
			send(client->getFd(), err.c_str(), err.length(), 0);
			return ;
		}
    }
    //connettiti
    this->incrementClient();
    this->voice_op_vec.push_back(client);
    clients.push_back(client);
}

void Channel::disconnect(Client* client)
{
    //qua va in seg/bus error
    if (std::find(clients.begin(), clients.end(), client) != clients.end())
        clients.erase(std::find(clients.begin(), clients.end(), client));
    this->decrementClient();
    //std::cout << this->nClient << std::endl;
    //non sei piu client di questo channel
    //tolgo i permessi da admin se abbandoni
    if (std::find(op_vec.begin(), op_vec.end(), client) != op_vec.end())
        op_vec.erase(std::find(op_vec.begin(), op_vec.end(), client));
    if (std::find(half_op_vec.begin(), half_op_vec.end(), client) != half_op_vec.end())
        half_op_vec.erase(std::find(half_op_vec.begin(), half_op_vec.end(), client));
    if (std::find(voice_op_vec.begin(), voice_op_vec.end(), client) != voice_op_vec.end())
        voice_op_vec.erase(std::find(voice_op_vec.begin(), voice_op_vec.end(), client));
    //controllo se c'é almeno un admin sennó almeno un admin ci deve essere
    if (op_vec.empty() && !clients.empty())
        op_vec.push_back(clients.at(0));
}

void Channel::incrementClient()
{
    this->nClient += 1;
}

void Channel::decrementClient()
{
    this->nClient -= 1;
}