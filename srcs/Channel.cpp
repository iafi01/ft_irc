#include "../includes/Channel.hpp"

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

bool Channel::ban(Client *client, std::string _reason)
{
    try {
        Banned victim;

        time_t now = time(0);

        victim.user = client;
        victim.reason = _reason;
        victim.ban_time = ctime(&now);
        this->banned_vec.push_back(&victim);
        return (true);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return (false);
    }
}

bool Channel::unBan(Client *client, std::string _reason)
{
    std::vector<Client*>::iterator i;

    i = clients.begin();
    while (i != clients.end())
    {
        if ((*i) == client)
        {   
            try {
                Banned victim;

                time_t now = time(0);

                victim.user = client;
                victim.reason = _reason;
                victim.ban_time = ctime(&now);
                this->banned_vec.push_back(&victim);
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

bool Channel::modInvite()
{

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

std::vector<Banned*> Channel::getBanned() const
{
    return (banned_vec);
}


//cmds
bool Channel::kickCmd(Client *client)
{

}

bool Channel::inviteCmd(Client *client)
{

}

bool Channel::topicCmd(std::string)
{

}

bool Channel::modeCmd(enum modOp type)
{

}
