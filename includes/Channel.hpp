#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Client.hpp"

class Channel {
    private:
        typedef struct Banned {
            Client* user;
            std::string reason;
        };
        enum {
            open,
            only_invite_by_all,
            only_invite_by_admin
        };
        enum {
            op,
            deop,
            halfOp,
            deHalfOp,
            voiceOp,
            deVoiceOp,
            ban,
            unBan
        };
        std::string name;
        std::string topic;
        std::vector<Client*> clients;
        std::vector<Client*> op;
        std::vector<Client*> half_op;
        std::vector<Client*> voice_op;
        std::vector<Banned*> banned;
        std::vector<Client*> invited;
        int n_users;

        //only admin functions
        bool op();
        bool deop();
        bool halfOp();
        bool deHalfOp();
        bool voiceOp();
        bool deVoiceOp();
        bool ban();
        bool unBan();
        bool modInvite();
    public:
        std::string getName() const;
        std::vector<Client*> getClients() const;
        std::vector<Client*> getOp() const;
        std::vector<Client*> halfOp() const;
        std::vector<Client*> voiceOp() const;
        std::vector<Banned*> getBanned() const;

        //cmds
        bool kick(Client *client);
        bool invite(Client *client);
        bool topic(std::string);
        bool mode(enum func);
};