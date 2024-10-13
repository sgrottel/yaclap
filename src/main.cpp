// yaclap.cpp : Defines the entry point for the application.
//

#include "yaclap.hpp"

#if defined(_WINDOWS) || defined(_WIN32)
#ifndef _WIN32
#define _WIN32
#endif
#include <Windows.h>
#include <tchar.h>
#endif

#include <iostream>

#ifdef _WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	std::cout << "Hello CMake." << std::endl;
	return 0;
}
