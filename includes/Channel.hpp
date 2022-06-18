#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <ctime>
#include <exception>
#include <algorithm>

#include "Client.hpp"

class Channel {
    private:
        typedef struct Banned {
            std::string nick;
            std::string user;
            std::string host;
            std::string reason;
            std::string ban_time;
            Client* admin;
        };
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
        std::vector<Banned*> banned_vec;
        int userLimit;
        int nClient;
        int is_only_invite;

        //only admin functions
        bool setPass(std::string psw);
        bool op(Client *client);
        bool deop(Client *client);
        bool halfOp(Client *client);
        bool deHalfOp(Client *client);
        bool voiceOp(Client *client);
        bool deVoiceOp(Client *client);
        bool ban(std::string nick, std::string user, std::string host, std::string _reason);
        bool unBan(std::string nick, std::string user, std::string host);
        bool modInvite(enum modInvite i);
    public:
        std::string getName() const;
        std::string getTopic() const;
        std::vector<Client*> getClients() const;
        std::vector<Client*> getOp() const;
        std::vector<Client*> halfOp() const;
        std::vector<Client*> voiceOp() const;
        std::vector<Banned*> getBanned() const;
        bool isInvited(const Client* client);
        bool isOp(const Client* client);
        bool isHalfOp(const Client* client);
        bool isVoiceOp(const Client* client);
        bool isBanned(const Client* client);
        bool invite(const Client* client);
        bool removeInvite(const Client* client);
        void connect(const Client* client, std::string psw);
        void disconnect(const Client* client);
        void sendMessage(const Client* sender, std::string msg) const;

        //cmds
        bool kickCmd(Client *client, std::string _reason);
        bool inviteCmd(Client *client);
        bool topicCmd(std::string setTopic);
        bool modeCmd(enum modOp type, Client *client, std::string _reason);
};