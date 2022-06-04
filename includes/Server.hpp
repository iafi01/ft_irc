#pragma once

#include <iostream>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <map>
#include <string>
#include <ctime>
#include <vector>

#include "Client.hpp"

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
        time_t now; //settare = time(0)
        std::string time_string; //ctime(&now) (localdate not UTC)
        
        //funzioni usate dal costruttore
        void setup_server(int port, std::string pass);
        void start_server();
    public:
        Server();
        ~Server();
        Server(const int port, const std::string pass);
        Server(const Server &obj);
        Server& operator=(const Server &obj);

        void accept_client();
        void send_all(std::vector<Client*> receivers, std::string mex, Client sender);
        
        //setters
        void setDate();
        void setPass();

        //getters
        int get_port() const;
        int get_sock() const;
        int get_max_id() const;
        std::string get_pass() const;
        std::string getDate() const;

        //commands

        //clients and channels management by server
};