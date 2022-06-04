#pragma once

#include <string>
#include <iostream>

#define ERR_ARG "Wrong number of arguments\n"
#define ERR_FAT "Fatal error\n"

class Exception {
    public:
        void fatal();
};