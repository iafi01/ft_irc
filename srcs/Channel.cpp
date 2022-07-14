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
    std::vector<Client *>::iterator i;

    i = clients.begin();
    while (i != clients.end())
    {
        if ((*i)->getNick() == client->getNick())
        {
            std::cout << "Nick di *i " << (*i)->getNick() << std::endl;
            std::cout << "Nick di C" << client->getNick() << std::endl;
            if(this->isOp(client))
            {
                std::cout << "isOp" << std::endl;
                //this->deop(client);
                std::cout << "fine isOp" << std::endl;
            }
            if(this->isHalfOp(client))
            {
                std::cout << "isHalf" << std::endl;
                this->half_op_vec.erase(i);
                std::cout << "fine isHalf" << std::endl;
            }
            if(this->isInvited(client))
            {
                std::cout << "isInvited" << std::endl;
                this->invited_vec.erase(i);
                std::cout << "fine isInvited" << std::endl;
            }
            if(this->isVoiceOp(client))
            {
                std::cout << "isVoice" << std::endl;
                this->voice_op_vec.erase(i);
                std::cout << "fine isVoice" << std::endl;
            }
            this->clients.erase(i);
            return ;//(true);
        }
        i++;
    }
    return ;//(false);
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

bool Channel::deop(Client *client)
{
    std::vector<Client*>::iterator i;

    i = clients.begin();
    while (i != clients.end())
    {
        if ((*i) == client)
        {   
            try {
                this->op_vec.erase(i);
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

bool Channel::halfOp(Client *client)
{
    try {
        this->half_op_vec.push_back(client);
        std::cout << "Halfoppato" <<std::endl;
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

bool Channel::deHalfOp(Client *client)
{
    std::vector<Client*>::iterator i;

    i = clients.begin();
    while (i != clients.end())
    {
        if ((*i) == client)
        {   
            try {
                this->half_op_vec.erase(i);
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
    std::vector<Client*>::iterator i;

    i = clients.begin();
    while (i != clients.end())
    {
        if ((*i) == client)
        {   
            try {
                this->voice_op_vec.erase(i);
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

bool Channel::ban(Client *admin, std::string nick = "*", std::string user = "*", std::string host = "*", std::string _reason = "")
{
    try {
        Banned victim;

        time_t now = time(0);

        victim.nick = nick;
        victim.user = user;
        victim.host = host;

        victim.reason = _reason;
        victim.ban_time = ctime(&now);
        victim.admin = admin;

        this->banned_vec.push_back(&victim);
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

bool Channel::unBan(std::string nick = "*", std::string user = "*", std::string host = "*")
{
    std::vector<Banned*>::iterator i;

    int j = 0;
    i = banned_vec.begin();
    while (i != banned_vec.end())
    {
        if (banned_vec.at(j)->nick == nick && banned_vec.at(j)->user == user && banned_vec.at(j)->host == host)
        {
            try {
                this->banned_vec.erase(i);
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
std::vector<Channel::Banned*> Channel::getBanned() const
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

// bool Channel::modeCmd(enum modOp type, Client *client, std::string _reason = "")
// {
//     try
//     {
//         if (type == _op)
//             op(client);
//         else if (type == _deop)
//             deop(client);
//         else if (type == _halfOp)
//             halfOp(client);
//         else if (type == _deHalfOp)
//             deHalfOp(client);
//         else if (type == _voiceOp)
//             voiceOp(client);
//         else if (type == _deVoiceOp)
//             deVoiceOp(client);
//         else if (type == _ban)
//             ban(client->getNick(), client->getUser(), client->getHost(), , _reason);
//         else if (type == _unBan)
//             unBan(client->getNick(), client->getUser(), client->getHost());
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//         return false;
//     }
//     return true;
// }

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
    std::vector<Banned*>::iterator i;

    int j = 0;
    i = banned_vec.begin();
    while (i != banned_vec.end())
    {
        if (banned_vec.at(j)->user == client->getUser())
        {
            return (true);
        }
        i++;
        j++;
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
    std::vector<Banned*>::iterator i;
    std::string err;
    if(!banned_vec.empty())
    {
        i = banned_vec.begin();
        //se sei bannato esce dalla funzione
        while (i != banned_vec.end())
        {
            if ((client->getNick() == (*i)->nick || client->getNick() == "*") &&
                (client->getUser() == (*i)->user || client->getUser() == "*") &&
                (client->getHost() == (*i)->host || client->getHost() == "*"))
                return ;
            i++;
        }
    }
    if (pass != psw)
    {
        err = "pass of the channel incorrect\n";
        send(client->getFd(), err.c_str(), err.length(), 0);
        return ;
    }
    if (nClient + 1 >= userLimit)
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
    std::cout << "numero_clienti ora:" << nClient << std::endl;
    clients.push_back(client);
}

void Channel::disconnect(Client* client)
{
    //qua va in seg/bus error
    std::vector<Client*>::iterator i;
    for (std::vector<Client*>::iterator j = clients.begin(); j != clients.end(); j++)
    {
        if (*j == client)
        {
            i = j;
            break;
        }
    }
    this->decrementClient();
    std::cout << this->nClient << std::endl;
    //non sei piu client di questo channel
    clients.erase(i);
    //tolgo i permessi da admin se abbandoni
    if (std::find(op_vec.begin(), op_vec.end(), client) != op_vec.end())
        op_vec.erase(i);
    if (std::find(half_op_vec.begin(), half_op_vec.end(), client) != half_op_vec.end())
        half_op_vec.erase(i);
    if (std::find(voice_op_vec.begin(), voice_op_vec.end(), client) != voice_op_vec.end())
        voice_op_vec.erase(i);
    //controllo se c'é almeno un admin sennó almeno un admin ci deve essere
    if (op_vec.empty())
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