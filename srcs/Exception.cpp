#include "Exception.hpp"

void Exception::fatal()
{
	std::perror(ERR_FAT);
	exit(1);
}