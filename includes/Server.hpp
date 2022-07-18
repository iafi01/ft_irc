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
#include <arpa/inet.h>

#include "Channel.hpp"
#include "Client.hpp"

#define ERR_ARG "Wrong number of arguments\n"
#define ERR_FAT "Fatal error\n"

/*
Per testare il programma scriviamo sul terminale, dopo aver avviato il server,:
>nc 0.0.0.0 8080

***NAHHH - Scrivere messaggio di benvenuto contenete le informazioni riguardo i comandi disponibili***
aggiustare il quit
privmsg fa scrivere ai bannati

//facoltativo spostare buona parte dei commands di channel in server per unificarli
//facoltativo aggiungeree piú files cpp per ogni classe: example (comandi, getters, setters ecc)
*/

class Client;

class Server
{
    private:
        std::string server_name;
        struct sockaddr_in serveraddr;
        int addrlen;
        std::string pass;
        int port;
        int sockfd;
        int max_id;
        int max_fd;
        int opt;
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
        void fatal(std::string s);
        
    public:
        Server();
        ~Server();
        Server(const int port, const std::string password);
/**/    Server(const Server &obj);
/**/    Server& operator=(const Server &obj);

        void accept_client();
        void send_all(std::string mex, Client sender); //Sends a message to every connected clients
        void clientRegister(Client *client);
        void eraseClient(int fd);

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
        std::vector<std::string> parseBanMask(std::string banMask);//function parse mode cmds banmask
        std::vector<Channel *> channelConvert(std::vector<std::string> splitted);
        bool compNames(std::string receiver, std::string nickname);
        std::string printTime();

        //void forceQuit(int fd);
        bool check_nick(Client *new_client, char *buffer, int valread);
        bool check_user(Client *new_client, char *buffer, int valread);
        bool check_pass(Client *new_client, char *buffer, int valread);


        //commands 
        bool parse_commands(Client *client, char *buf, int valrecv); //Function used to parse commands sent by clients
        

        //channel cmd
        void mode_cmd(Client *client, std::vector<std::string> splitted);
        void invite_cmd(std::vector<Client *> invited, std::string channel_name, Client *sender);
        void topic_cmd(std::string channel_name, std::vector<std::string> splitted, Client *sender);
        void kick_cmd(std::string channel_name, std::string client_name, Client *sender, std::string reason);
        bool quit_cmd(Client *client, std::vector<std::string> words);

        //other cmd
        void join_cmd(Client *client, std::string channel_name, std::string psw);
        void op_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToOp);
        void deop_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToDeOp);
        void half_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToHalfOp);
        void dehalf_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToDeHalfOp);
        void ban_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToBan, std::string reason);
        void unban_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToUnBan);
        void voice_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToVoice);
        void unvoice_cmd(Client *admin, std::string channel_name, std::vector<Client *> clientToUnVoice);
        void who_cmd(std::string filter, Client *client);   //The filter is the name of the channel that contains the list of users that you want to visualize [channel or a client] (# or ' ')
        //void whois_cmd(std::vector<std::string> splitted, Client *sender);
        void privmsg_cmd(Client *admin, std::string target, std::vector<std::string> text); //The target could be a channel or a client (# or ' ')
        void part_cmd(Client *client, std::vector<std::string > splitted); //Leaves a channel/s
        //void pass_cmd(Client *admin, std::string pass);

        void notQuitCmd(int sd, int i); //sotto_cmd riferito al quit
        //clients and channels management by server
        Client *getClient(int sockfd);
        int findIterClient(Client *client);
        Channel* getChannel(std::string nameCh);
        void addChannel(Channel *toAdd);
        Client *getClientFromUser(std::string username, std::vector<Client *> clie);
};