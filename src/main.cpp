// yaclap.cpp : Defines the entry point for the application.
//

#if defined(_WINDOWS) || defined(_WIN32)
#ifndef _WIN32
#define _WIN32
#endif
#include <Windows.h>
#include <tchar.h>
#endif

#include "cmdargs.h"

#include <iostream>

#ifdef _WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	Config cfg;
	bool parseOk = cfg.ParseCmdLine(argc, argv);

	std::cout << "::::";
	if (parseOk)
	{
		std::cout << "o";
		switch (cfg.m_cmd)
		{
		case Command::None: std::cout << "n"; break;
		case Command::CommandA: std::cout << "A"; break;
		case Command::CommandB: std::cout << "B"; break;
		default: std::cout << "x"; break;
		}
		std::cout
			<< cfg.m_verbose
			<< ((cfg.m_input != nullptr) ? "i" : "_")
			<< cfg.m_value
			<< ((cfg.m_andArg != nullptr) ? "a" : "_");
		if (cfg.m_input != nullptr)
		{
			std::wcout << L"\n"
				<< cfg.m_input;
		}
		if (cfg.m_andArg != nullptr)
		{
			std::wcout << L"\n"
				<< cfg.m_andArg;
		}
		std::cout << std::endl;
	}
	else
	{
		std::cout << "f-----" << std::endl;
	}

	return parseOk ? 0 : 1;
}
