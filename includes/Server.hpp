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
#include <algorithm>

#include "Channel.hpp"
#include "Client.hpp"

#define ERR_ARG "Wrong number of arguments\n"
#define ERR_FAT "Fatal error\n"

/*
Per testare il programma scriviamo sul terminale, dopo aver avviato il server,:
>nc 0.0.0.0 8080
*/

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
        Server(const int port, const std::string password);
/**/    Server(const Server &obj);
/**/    Server& operator=(const Server &obj);

        void accept_client(int sockfd);
        void send_all(std::string mex, Client sender); //Sends a message to every connected clients
        
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
        std::string toUpper(std::string toUp); //Convert a character from lower case to upper case
        bool compStr(std::string buf, std::string str); //Compare two given strings
        std::vector<Client *> clientConvert(std::vector<std::string> toConv); //Return a vector of clients, they are extrapolated from a splitted string (std::vector<std::string>)
        std::string topicConvert(std::vector<std::string> toConv); //Return a string that contains the topic of a channel, same as clientConvert
        
        //commands 
        bool parse_commands(Client *client, char *buf, int valrecv); //Function used to parse commands sent by clients
        bool quit_cmd();

        //channel cmd
        bool mode_cmd();
        bool invite_cmd(std::vector<Client *> invited, std::string channel_name);
        bool topic_cmd(std::string channel_name, std::string topic);
        bool kick_cmd(std::string channel_name, std::string client_name, std::string reason);

        //other cmd
        bool join_cmd(Client *client, std::string channel_name, std::string psw);
        bool op_cmd();
        bool deop_cmd();
        bool half_cmd();
        bool dehalf_cmd();
        bool ban_cmd();
        bool unban_cmd();
        bool voice_cmd();
        bool unvoice_cmd();
        bool who_cmd();
        bool whois_cmd();
        bool privmsg_cmd();
        bool mode_cmd();

        //clients and channels management by server
        Client getClient(int sockfd);
        Channel getChannel(std::string nameCh);
};