#pragma once

#include <tchar.h>
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

    // value for command B
    int m_value = 0;

    // an additional, positional argument for command B
    std::basic_string<_TCHAR> m_andArg;

    bool ParseCmdLine(int argc, const _TCHAR* const* argv);
};
