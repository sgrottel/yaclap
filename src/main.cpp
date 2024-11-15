// yaclap.cpp : Defines the entry point for the application.
//

#if defined(_WINDOWS) || defined(_WIN32)
#ifndef _WIN32
#define _WIN32
#endif
#include <Windows.h>
#include <tchar.h>
#else
#define _TCHAR char
#define _T(A) A
#endif

#include "cmdargs.hpp"

#include <iostream>

#ifdef _WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    Config cfg;
    bool parseOk = cfg.ParseCmdLine(argc, argv);

    std::wcout << L"::info::"
#if defined(DEBUG) || defined(_DEBUG)
              << L"DEBUG:"
#else
              << L"RELEASE:"
#endif
#if defined(NDEBUG)
              << L"NDEBUG:"
#else
              << L":"
#endif
              << (sizeof(void*) * 8) << L":"
              << sizeof(_TCHAR) << L"\n";
    cfg.PrintVersionInfo();

    std::wcout << L"::::";
    if (parseOk)
    {
        std::wcout << L"o";
        switch (cfg.m_cmd)
        {
            case Command::None:
                std::wcout << L"n";
                break;
            case Command::CommandA:
                std::wcout << L"A";
                break;
            case Command::CommandB:
                std::wcout << L"B";
                break;
            default:
                std::wcout << L"x";
                break;
        }
        std::wcout << cfg.m_verbose << ((cfg.m_input != nullptr) ? L"i" : L"_") << cfg.m_intValue << L"_"
                   << cfg.m_doubleValue << L"_" << (cfg.m_boolValue ? L"t" : L"f") << L"_"
                   << ((!cfg.m_andArg.empty()) ? L"a" : L"_");
        if (cfg.m_input != nullptr)
        {
            std::wcout << L"\n" << cfg.m_input;
        }
        if (!cfg.m_andArg.empty())
        {
            std::wcout << L"\n" << cfg.m_andArg.c_str();
        }
        std::wcout << std::endl;
    }
    else
    {
        std::wcout << L"f-----" << std::endl;
    }

    return parseOk ? 0 : 1;
}
