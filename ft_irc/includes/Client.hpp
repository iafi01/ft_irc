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
        std::string hostname;
        bool isLogged;
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
        int         getIsLogged() const;
        std::string getNick() const;
        std::string getUser() const;
        std::string getHost() const;

        //setters
        void setNick(std::string str);
		void setUser(std::string str);
        void setIsMsg(int i);
        void setFd(int fd);
        void setId(int id);
        void setIsLogged(bool log);
};