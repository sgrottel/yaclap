#pragma once

#ifdef _WIN32
#include <tchar.h>
#else
#define _TCHAR char
#define _T(A) A
#endif
#include <string>

enum class Command
{
    None,
    CommandA,
    CommandB
};

struct Config
{

    // selected subcommand, if any
    Command m_cmd = Command::None;

    // number of verbose calls (all commands)
    int m_verbose = 0;

    // input for command A
    const _TCHAR* m_input = nullptr;

    // integer value for command B
    long long m_intValue = 0;

    // double value for command B
    double m_doubleValue = 0.0;

    // bool value for command B
    bool m_boolValue = false;

    // an additional, positional argument for command B
    std::basic_string<_TCHAR> m_andArg;

    bool ParseCmdLine(int argc, const _TCHAR* const* argv);

    void PrintVersionInfo();

};
