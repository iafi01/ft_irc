#pragma once

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

#define ERR_ARG "Wrong number of arguments\n"
#define ERR_FAT "Fatal error\n"

class Client
{
    private:
        std::string nickname;
        std::string username;   // !!! Lo username e' statico !!!
        int fd;
        int id;
        int is_msg;      //Variabile che indica se lo user ha appena inviato un messaggio
    public:
        Client();
        ~Client();
        Client(int fd);
        Client(const Client &obj);
        Client& operator=(const Client &obj);
        //getters
        int         getFd() const;
        int         getId() const;
        int         getIsMsg() const;
        std::string getNick() const;
        std::string getUser() const;

        //setters
        void setIsMsg(int i);
        Client getClient(int fd);
        
};