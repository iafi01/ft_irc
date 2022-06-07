#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <ctime>
#include <exception>

#include "Client.hpp"

class Channel {
    private:
        typedef struct Banned {
            Client* user;
            std::string reason;
            std::string ban_time;
            Client* admin;
            int permanent; //kick = 0 or ban = 1
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
        std::vector<Client*> clients;
        std::vector<Client*> op_vec;
        std::vector<Client*> half_op_vec;
        std::vector<Client*> voice_op_vec;
        std::vector<Banned*> banned_vec;
        std::vector<Client*> invited_vec;
        int n_users;
        int is_only_invite;

        //only admin functions
        bool op(Client *client);
        bool deop(Client *client);
        bool halfOp(Client *client);
        bool deHalfOp(Client *client);
        bool voiceOp(Client *client);
        bool deVoiceOp(Client *client);
        bool ban(Client *client, std::string _reason);
        bool unBan(Client *client);
        bool modInvite(enum modInvite i);
    public:
        std::string getName() const;
        std::string getTopic() const;
        std::vector<Client*> getClients() const;
        std::vector<Client*> getOp() const;
        std::vector<Client*> halfOp() const;
        std::vector<Client*> voiceOp() const;
        std::vector<Banned*> getBanned() const;

        //cmds
        bool kickCmd(Client *client, std::string _reason);
        bool inviteCmd(Client *client);
        bool topicCmd(std::string setTopic);
        bool modeCmd(enum modOp type, Client *client, std::string _reason);
};