#include "../includes/Channel.hpp"

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

bool Channel::ban(std::string nick = "*", std::string user = "*", std::string host = "*", std::string _reason = "")
{
    try {
        Banned victim;

        time_t now = time(0);

        victim.nick = nick;
        victim.user = user;
        victim.host = host;

        victim.reason = _reason;
        victim.ban_time = ctime(&now);
        victim.permanent = 1;
        //victim.admin->nickname = ;
        //victim.admin->username = ;
        //victim.admin->hostname = ;
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


//cmds
bool Channel::kickCmd(Client *client, std::string _reason)
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

bool Channel::modeCmd(enum modOp type, Client *client, std::string _reason = "")
{
    try
    {
        if (type == _op)
            op(client);
        else if (type == _deop)
            deop(client);
        else if (type == _halfOp)
            halfOp(client);
        else if (type == _deHalfOp)
            deHalfOp(client);
        else if (type == _voiceOp)
            voiceOp(client);
        else if (type == _deVoiceOp)
            deVoiceOp(client);
        else if (type == _ban)
            ban(client, _reason);
        else if (type == _unBan)
            unBan(client);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
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
    std::vector<Client*>::iterator i;

    i = op_vec.begin();
    while (i != op_vec.end())
    {
        if ((*i) == client)
            return (true);
        i++;
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
        if (banned_vec.at(j)->user == client)
        {
            return (true);
        }
        i++;
        j++;
    }
    return (false);
}

bool Channel::invite(const Client* client)
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

void Channel::connect(const Client* client, std::string psw = "")
{
    std::vector<Banned*>::iterator i;

    i = banned_vec.begin();
    //se sei bannato esce dalla funzione
    while (i != banned_vec.end())
    {
        if ((client->getNick() == (*i)->nick || client->getNick() == "*") &&
            (client->getUser() == (*i)->user || client->getUser() == "*") &&
            (client->getHost() == (*i)->host || client->getHost() == "*"))
            return ;
    }
    if (pass != pwd)
        return ;
    if (nClient + 1 >= userLimit)
        return ;
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
            return;
    }
    //connettiti
    nClient++;
    clients.push_back(client);
}

void Channel::disconnect(const Client* client)
{
    std::vector<Client*>::iterator i;

    i = client;

    //tolgo i permessi da admin se abbandoni?
    //controllo se c'é almeno un admin sennó almeno un admin ci deve essere
    nClient--;
    clients.erase(i);
}

void Channel::sendMessage(const Client* sender, std::string msg) const
{

}

bool Channel::kickCmd(Client *client, std::string _reason)
{

}

bool Channel::inviteCmd(Client *client)
{

}

bool Channel::topicCmd(std::string setTopic)
{

}

bool Channel::modeCmd(enum modOp type, Client *client, std::string _reason)
{

}
