#pragma once

#include <iostream>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <map>
#include <string>
#include <ctime>
#include <vector>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>

#include "Channel.hpp"
#include "Client.hpp"

#define ERR_ARG "Wrong number of arguments\n"
#define ERR_FAT "Fatal error\n"

class Client;

class Server
{
    private:
        struct sockaddr_in serveraddr;
        std::string pass;
        int port;
        int sockfd;
        int max_id;
        fd_set curr_fds, read_fds, write_fds;
        char server_buffer[64];
        std::map<int, Client*> client_map;
        std::map<std::string, Channel*> channel_map;
        time_t now; //settare = time(0)
        std::string time_string; //ctime(&now) (localdate not UTC)
        std::vector<Client*> clients;

        //funzioni usate dal costruttore
        void setup_server(int port, std::string pass);
        void start_server();
        void fatal();
        
    public:
        Server();
        ~Server();
        Server(const int port, const std::string pass);
/**/    Server(const Server &obj);
/**/    Server& operator=(const Server &obj);

        void accept_client(int sockfd);
        void send_all(std::string mex, Client sender);
        
        //setters
        void setDate();
        void setPass(std::string new_pass);

        //getters
        int get_port() const;
        int get_sock() const;
        int get_max_id() const;
        std::string get_pass() const;
        std::string getDate() const;

        //utils
        int get_max_fd(int sockfd);
        //commands
        void parse_commands(Client *client, std::string cmd);
        void quit_cmd();
        //channel cmd
        void mode_cmd();
        void invite_cmd();
        void topic_cmd();
        void kick_cmd();
        //other cmd
        void join_cmd();
        //clients and channels management by server
        Client getClient(int sockfd);
        Channel getChannel(std::string nameCh);
};