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
            //Client *admin;
        }   Banned;
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
        std::vector<Client*> op_vec;
        std::vector<Client*> half_op_vec;
        std::vector<Client*> voice_op_vec;
        std::vector<Banned*> banned_vec;
        std::vector<Client*> invited_vec;
        int n_users;

        //only admin functions
        bool op(Client *client);
        bool deop(Client *client);
        bool halfOp(Client *client);
        bool deHalfOp(Client *client);
        bool voiceOp(Client *client);
        bool deVoiceOp(Client *client);
        bool ban(Client *client, std::string _reason);
        bool unBan(Client *client, std::string _reason);
        bool modInvite();
    public:
        std::string getName() const;
        std::string getTopic() const;
        std::vector<Client*> getClients() const;
        std::vector<Client*> getOp() const;
        std::vector<Client*> halfOp() const;
        std::vector<Client*> voiceOp() const;
        std::vector<Banned*> getBanned() const;

        //cmds
        bool kickCmd(Client *client);
        bool inviteCmd(Client *client);
        bool topicCmd(std::string);
        bool modeCmd(enum func);
};