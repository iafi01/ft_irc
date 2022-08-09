#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <ctime>
#include <exception>
#include <algorithm>
#include <sys/time.h>

#include "Client.hpp"
//#include "Server.hpp"

class Channel {
    private:
        typedef struct Banned {
            std::string nick;
            std::string user;
            std::string host;
            std::string reason;
            std::string ban_time;
            Client* admin;
        }              Banned;
        enum modInvite{
            open,
            only_invite_by_all,
            only_invite_by_admin
        };
        enum modOp{
            _op,
            _deop,
            _halfOp,
            _deHalfOp,
            _voiceOp,
            _deVoiceOp,
            _ban,
            _unBan
        };
        std::string name;
        std::string topic;
        std::string pass;
        std::vector<Client*> clients;
        std::vector<Client*> op_vec;
        std::vector<Client*> half_op_vec;
        std::vector<Client*> voice_op_vec;
        std::vector<Client*> invited_vec;
        std::vector<Banned> banned_vec;
        int userLimit;
        int nClient;
        int is_only_invite;
        int timeCreation;

    public:
        
        Channel();
        ~Channel();
        Channel(std::string name, int userLimit, int is_only_invite, std::string psw, std::string topic);

        //only admin functions
        bool setPass(std::string psw);
        void removeClient(Client *client);
        bool op(Client *client);
        void deop(Client *client);
        bool halfOp(Client *client);
        bool deHalfOp(Client *client);
        bool voiceOp(Client *client);
        bool deVoiceOp(Client *client);
        bool ban(Client *admin, std::string nick, std::string user, std::string host, std::string _reason);
        bool unBan(std::string nick, std::string user, std::string host);
        bool modInvite(enum modInvite i);
        
        std::string getName() const;
        std::string getTopic() const;
        bool setTopic(std::string topic);
        int retTime();
        int	getTime();

        std::vector<Client*> getClients() const;
        std::vector<Client*> getOp() const;
        std::vector<Client*> halfOp() const;
        std::vector<Client*> voiceOp() const;
        std::vector<Banned> getBanned() const;
        bool isClient(const Client* client);
        bool isInvited(const Client* client);
        bool isOp(const Client* client);
        bool isHalfOp(const Client* client);
        bool isVoiceOp(const Client* client);
        bool isBanned(const Client* client);
        bool invite(Client* client);
        bool removeInvite(const Client* client);
        void connect(Client* client, std::string psw);
        void disconnect(Client* client);

        //Clients operations
        void addClient(Client *toAdd);
        void incrementClient();
        void decrementClient();

        //cmds
        bool kickCmd(Client *client, std::string _reason);
        bool inviteCmd(Client *client);
        bool topicCmd(std::string setTopic);
        bool modeCmd(enum modOp type, Client *client, std::string _reason);
};