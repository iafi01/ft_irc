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

Prototipi dei comandi (per channel) - OK(penso)
Aggiungere parametri alle chiamate di funzione dei comandi - DA FINIRE
Scrivere le funzioni dei comandi - DA FARE
Scrivere la stampa dei messaggi a video dei vari comandi - DA FARE
Scrivere i controlli inerenti agli operatori (op, halfop) per i vari comandi - DA FARE
Il channel operator (op) puo' eseguire qualsiasi comando mentre l'halfop non puo' dare alcun
tipo di potere tranne voice e unvoice (gestire i permessi)
Scrivere parsing per MODE command
Scrivere messaggio di benvenuto contenete le informazioni riguardo i comandi disponibili
L'halfop, ogni qualvolta tentera' di togliere/mettere/kickare/bannare un channel operator,
ricevera' il messaggio "... 482 <nickname> <channel_name> :You are not channel operator"
//facoltativo aggiungeree piú files cpp per ogni classe: example (comandi, getters, setters ecc)
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
        int max_fd;
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
        std::vector<std::string> parseBanMask(std::string banMask);//function parse mode cmds banmask
        std::vector<Channel *> channelConvert(std::vector<std::string> splitted);

        //commands 
        bool parse_commands(Client *client, char *buf, int valrecv); //Function used to parse commands sent by clients
        

        //channel cmd
        void mode_cmd(Client *client, std::vector<std::string> splitted);
        void invite_cmd(std::vector<Client *> invited, std::string channel_name, Client *sender);
        void topic_cmd(std::string channel_name, std::vector<std::string> splitted, Client *sender);
        void kick_cmd(std::string channel_name, std::string client_name, std::string reason, Client *sender);
        void quit_cmd(Client *client, std::vector<std::string> words);

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
        void who_cmd(std::string filter, Client *sender);   //The filter is the name of the channel that contains the list of users that you want to visualize [channel or a client] (# or ' ')
        void whois_cmd(std::string nickname, Client *sender);
        void privmsg_cmd(Client *admin, std::string target, std::vector<std::string> text); //The target could be a channel or a client (# or ' ')
        void leave_cmd(Client *client, std::vector<Channel *> channel); //Leaves a channel/s
        void pass_cmd(Client *admin, std::string pass);

        //clients and channels management by server
        Client *getClient(int sockfd);
        std::vector<Client*>::iterator findIterClient(Client *client);
        Channel* getChannel(std::string nameCh);
};