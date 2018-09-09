#include "error.h"
#include <iostream> 

bool error = false; 

bool haserror()
{
	return error;
}

void throwerror(std::string text)
{
	error = true; 
	std::cout << text << '\n'; //todo: anything but this 
}
