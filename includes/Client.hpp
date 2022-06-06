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
        std::string username;
        int fd;
        int id;
        int is_msg;
    public:
        Client();
        ~Client();
        Client(int fd);
        Client(const Client &obj);
        Client& operator=(const Client &obj);
        Client get_client(int fd);

        //getters
        int get_fd() const;
        int get_id() const;
        int get_is_msg() const;
};