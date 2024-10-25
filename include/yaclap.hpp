//
// yaclap.hpp
// Yet Another Command Line Argument Parser
// https://github.com/sgrottel/yaclap
//
// MIT License
//
// Copyright(c) 2024 Sebastian Grottel
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef _YACLAP_HPP_INCLUDED_
#define _YACLAP_HPP_INCLUDED_

#pragma once

#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace yaclap
{

    template <typename CHAR>
    class Alias
    {
    public:
        enum class StringCompare
        {
            CaseSensitive,
            CaseInsensitive
        };

        Alias() noexcept = default;

        Alias(const CHAR* name, StringCompare stringCompare = StringCompare::CaseSensitive)
            : m_name{name}, m_stringCompare{stringCompare}
        {
        }

        template <typename T, typename A>
        Alias(const std::basic_string<CHAR, T, A>& name, StringCompare stringCompare = StringCompare::CaseSensitive)
            : m_name{name}, m_stringCompare{stringCompare}
        {
        }

        template <typename T>
        Alias(const std::basic_string_view<CHAR, T>& name, StringCompare stringCompare = StringCompare::CaseSensitive)
            : m_name{name}, m_stringCompare{stringCompare}
        {
        }

        const std::basic_string<CHAR>& GetName() const
        {
            return m_name;
        }

        StringCompare GetStringCompareMode() const
        {
            return m_stringCompare;
        }

    private:
        std::basic_string<CHAR> m_name;
        StringCompare m_stringCompare;
    };

    template <typename CHAR>
    class WithNameAndAlias
    {
    protected:
        WithNameAndAlias() noexcept = default;

        WithNameAndAlias(const Alias<CHAR>& name)
        {
            AddAliasImpl(name);
        }

        void AddAliasImpl(const Alias<CHAR>& alias)
        {
            if (alias.GetName().empty())
                throw std::invalid_argument("alias");
            m_names.push_back(alias);
        }

    public:
        inline std::vector<Alias<CHAR>>::const_iterator NameAliasBegin() const
        {
            return m_names.cbegin();
        }

        inline std::vector<Alias<CHAR>>::const_iterator NameAliasEnd() const
        {
            return m_names.cend();
        }

        std::basic_string<CHAR> GetAllNames() const;

    private:
        std::vector<Alias<CHAR>> m_names;
    };

    template <typename CHAR>
    class WithName
    {
    protected:
        WithName() noexcept = default;

        template <typename TSTR>
        WithName(const TSTR& name)
            : m_name{name}
        {
        }

    public:
        inline const std::basic_string<CHAR>& GetName() const noexcept
        {
            return m_name;
        }

    private:
        std::basic_string<CHAR> m_name;
    };

    template <typename CHAR>
    class WithDesciption
    {
    protected:
        WithDesciption() noexcept = default;

        template <typename TSTR>
        WithDesciption(const TSTR& description)
            : m_desc{description}
        {
        }

    public:
        inline const std::basic_string<CHAR>& GetDescription() const noexcept
        {
            return m_desc;
        }

    private:
        std::basic_string<CHAR> m_desc;
    };

    template <typename CHAR>
    class WithIdentity
    {
    private:
        static uint32_t c_nextId;
        uint32_t m_id = 0;

    public:
        WithIdentity() noexcept
            : m_id{c_nextId++}
        {
        }

        WithIdentity(const WithIdentity& src) noexcept
            : m_id{src.m_id}
        {
        }

        WithIdentity(WithIdentity&& src) noexcept
            : m_id{src.m_id}
        {
            src.m_id = c_nextId++;
        }

        WithIdentity& operator=(const WithIdentity& src) noexcept
        {
            m_id = src.m_id;
            return *this;
        }

        WithIdentity& operator=(WithIdentity&& src) noexcept
        {
            m_id = src.m_id;
            src.m_id = c_nextId++;
            return *this;
        }

        inline const auto GetId() const noexcept
        {
            return m_id;
        }

        template <typename A, typename B>
        static inline bool Equals(const A& a, const B& b) noexcept
        {
            return static_cast<WithIdentity<CHAR> const&>(a).m_id == static_cast<WithIdentity<CHAR> const&>(b).m_id;
        }
    };

    template <typename CHAR>
    uint32_t WithIdentity<CHAR>::c_nextId = 1;

    template <typename CHAR>
    class Argument : public WithIdentity<CHAR>, public WithName<CHAR>, public WithDesciption<CHAR>
    {
    public:
        static constexpr bool NotRequired = false;

        Argument() noexcept = default;

        template <typename TSTR1, typename TSTR2>
        explicit Argument(const TSTR1& name, const TSTR2& description, bool isRequired = true)
            : WithName<CHAR>{name}, WithDesciption<CHAR>{description}, m_isRequired{isRequired}
        {
        }

        inline bool IsRequired() const noexcept
        {
            return m_isRequired;
        }

    private:
        bool m_isRequired;
    };

    template <typename CHAR>
    class Option : public WithIdentity<CHAR>, public WithNameAndAlias<CHAR>, public WithDesciption<CHAR>
    {
    public:
        Option() noexcept = default;

        template <typename TSTR1, typename TSTR2>
        explicit Option(const Alias<CHAR>& name, const TSTR1& argumentName, const TSTR2& description)
            : WithNameAndAlias<CHAR>{name}, WithDesciption<CHAR>{description}, m_argName{argumentName}
        {
        }

        Option& AddAlias(const Alias<CHAR>& alias)
        {
            WithNameAndAlias<CHAR>::AddAliasImpl(alias);
            return *this;
        }

        inline const std::basic_string<CHAR>& GetArgumentName() const noexcept
        {
            return m_argName;
        }

    private:
        std::basic_string<CHAR> m_argName;
    };

    template <typename CHAR>
    class Switch : public WithIdentity<CHAR>, public WithNameAndAlias<CHAR>, public WithDesciption<CHAR>
    {
    public:
        Switch() noexcept = default;

        template <typename TSTR>
        explicit Switch(const Alias<CHAR>& name, const TSTR& description)
            : WithNameAndAlias<CHAR>{name}, WithDesciption<CHAR>{description}
        {
        }

        Switch& AddAlias(const Alias<CHAR>& alias)
        {
            WithNameAndAlias<CHAR>::AddAliasImpl(alias);
            return *this;
        }
    };

    template <typename CHAR>
    class Command;

    template <typename CHAR>
    class WithCommandContainer
    {
    protected:
        WithCommandContainer() noexcept = default;

        void AddCommandImpl(const Command<CHAR>& command);

        void AddOptionImpl(const Option<CHAR>& option)
        {
            if (option.NameAliasBegin() == option.NameAliasEnd())
                throw std::invalid_argument("option");
            m_options.push_back(option);
        }

        void AddSwitchImpl(const Switch<CHAR>& switchOption)
        {
            if (switchOption.NameAliasBegin() == switchOption.NameAliasEnd())
                throw std::invalid_argument("switchOption");
            m_switches.push_back(switchOption);
        }

        void AddArgumentImpl(const Argument<CHAR>& argument)
        {
            m_arguments.push_back(argument);
        }

    public:
        inline std::vector<Command<CHAR>>::const_iterator CommandsBegin() const
        {
            return m_commands.cbegin();
        }

        inline std::vector<Command<CHAR>>::const_iterator CommandsEnd() const
        {
            return m_commands.cend();
        }

        inline std::vector<Option<CHAR>>::const_iterator OptionsBegin() const
        {
            return m_options.cbegin();
        }

        inline std::vector<Option<CHAR>>::const_iterator OptionsEnd() const
        {
            return m_options.cend();
        }

        inline std::vector<Switch<CHAR>>::const_iterator SwitchesBegin() const
        {
            return m_switches.cbegin();
        }

        inline std::vector<Switch<CHAR>>::const_iterator SwitchesEnd() const
        {
            return m_switches.cend();
        }

        inline std::vector<Argument<CHAR>>::const_iterator ArgumentsBegin() const
        {
            return m_arguments.cbegin();
        }

        inline std::vector<Argument<CHAR>>::const_iterator ArgumentsEnd() const
        {
            return m_arguments.cend();
        }

    private:
        std::vector<Command<CHAR>> m_commands;
        std::vector<Option<CHAR>> m_options;
        std::vector<Switch<CHAR>> m_switches;
        std::vector<Argument<CHAR>> m_arguments;
    };

    template <typename CHAR>
    class Command : public WithIdentity<CHAR>,
                    public WithCommandContainer<CHAR>,
                    public WithNameAndAlias<CHAR>,
                    public WithDesciption<CHAR>
    {
    public:
        Command() noexcept = default;

        template <typename TSTR>
        Command(const Alias<CHAR>& name, const TSTR& description)
            : WithCommandContainer<CHAR>{}, WithNameAndAlias<CHAR>{name}, WithDesciption<CHAR>{description}
        {
        }

        Command& AddAlias(const Alias<CHAR>& alias)
        {
            WithNameAndAlias<CHAR>::AddAliasImpl(alias);
            return *this;
        }

        Command& AddCommand(const Command<CHAR>& command)
        {
            WithCommandContainer<CHAR>::AddCommandImpl(command);
            return *this;
        }

        Command& AddOption(const Option<CHAR>& option)
        {
            WithCommandContainer<CHAR>::AddOptionImpl(option);
            return *this;
        }

        Command& AddSwitch(const Switch<CHAR>& switchOption)
        {
            WithCommandContainer<CHAR>::AddSwitchImpl(switchOption);
            return *this;
        }

        Command& AddArgument(const Argument<CHAR>& argument)
        {
            WithCommandContainer<CHAR>::AddArgumentImpl(argument);
            return *this;
        }

        Command& Add(const Command<CHAR>& command)
        {
            return AddCommand(command);
        }

        Command& Add(const Option<CHAR>& option)
        {
            return AddOption(option);
        }

        Command& Add(const Switch<CHAR>& switchOption)
        {
            return AddSwitch(switchOption);
        }

        Command& Add(const Argument<CHAR>& argument)
        {
            return AddArgument(argument);
        }
    };

    template <typename CHAR>
    void WithCommandContainer<CHAR>::AddCommandImpl(const Command<CHAR>& command)
    {
        if (command.NameAliasBegin() == command.NameAliasEnd())
            throw std::invalid_argument("command");
        m_commands.push_back(command);
    }

    template <typename CHAR>
    class Parser : public WithCommandContainer<CHAR>, public WithName<CHAR>, public WithDesciption<CHAR>
    {
    public:
        template <typename TSTR1, typename TSTR2>
        Parser(const TSTR1& name, const TSTR2& description)
            : WithCommandContainer<CHAR>{}, WithName<CHAR>{name}, WithDesciption<CHAR>{description}
        {
        }

        Parser& AddCommand(const Command<CHAR>& command)
        {
            WithCommandContainer<CHAR>::AddCommandImpl(command);
            return *this;
        }

        Parser& AddOption(const Option<CHAR>& option)
        {
            WithCommandContainer<CHAR>::AddOptionImpl(option);
            return *this;
        }

        Parser& AddSwitch(const Switch<CHAR>& switchOption)
        {
            WithCommandContainer<CHAR>::AddSwitchImpl(switchOption);
            return *this;
        }

        Parser& AddArgument(const Argument<CHAR>& argument)
        {
            WithCommandContainer<CHAR>::AddArgumentImpl(argument);
            return *this;
        }

        Parser& Add(const Command<CHAR>& command)
        {
            return AddCommand(command);
        }

        Parser& Add(const Option<CHAR>& option)
        {
            return AddOption(option);
        }

        Parser& Add(const Switch<CHAR>& switchOption)
        {
            return AddSwitch(switchOption);
        }

        Parser& Add(const Argument<CHAR>& argument)
        {
            return AddArgument(argument);
        }

        inline void EnableImplicitHelpSwitch(bool enable = true) noexcept
        {
            m_withImplicitHelpSwitch = enable;
        }

        inline bool IsImplicitHelpSwitchEnabled() const noexcept
        {
            return m_withImplicitHelpSwitch;
        }

        /// <summary>
        /// The parse result only identifies commands, options, switches, and arguments.
        /// Use additional calls on this object to convert and assign values.
        /// </summary>
        class Result
        {
        public:
            /// <summary>
            /// Returns true if this is the Result of a successful parsing,
            /// i.e. no errors were encountered, and the implicit help switch was not triggered.
            /// </summary>
            inline bool IsSuccess() const noexcept
            {
                return m_success;
            }

            /// <summary>
            /// Returns true if the help information should be shown after assigning all results
            /// </summary>
            inline bool ShouldShowHelp() const noexcept
            {
                return m_shouldShowHelp;
            }

            /// <summary>
            /// Sets the error message to be shown to the user
            /// </summary>
            /// <param name="message">Use an ANSI/ASCII string, or a similar byte encoding,
            /// which matches the encoding of the terminal your application is running in</param>
            inline void SetError(const char* message, bool setUnsuccessful = true)
            {
                m_error = message;
                if (setUnsuccessful)
                {
                    m_success = false;
                    m_shouldShowHelp = true;
                }
            }

            /// <summary>
            /// Gets the set error message
            /// </summary>
            inline std::string const& GetError() const noexcept
            {
                return m_error;
            }

            /// <summary>
            /// Prints to set error message to std out
            /// </summary>
            inline void PrintError(bool tryUseColor = true) const;

            /// <summary>
            /// Prints the error message to the specified stream
            /// </summary>
            template <typename TSTREAMT = std::basic_ostream<CHAR>::traits_type>
            void PrintError(std::basic_ostream<CHAR, TSTREAMT>& stream, bool tryUseColor = true) const;

            // TODO: Implement

        protected:
            Result() = default;

        private:
            bool m_success = false;
            bool m_shouldShowHelp = false;
            std::string m_error{};
        };

        /// <summary>
        /// Parses the specified command line and returns the parse result
        /// </summary>
        Result Parse(int argc, const CHAR* const* argv, bool skipFirstArg = true) const;

        /// <summary>
        /// Prints a user-readable help text
        /// </summary>
        template <typename TSTREAMT = std::basic_ostream<CHAR>::traits_type>
        inline void PrintHelp(Command<CHAR> const& command, std::basic_ostream<CHAR, TSTREAMT>& stream) const;

        /// <summary>
        /// Prints a user-readable help text
        /// </summary>
        template <typename TSTREAMT = std::basic_ostream<CHAR>::traits_type>
        inline void PrintHelp(Result& result, std::basic_ostream<CHAR, TSTREAMT>& stream) const;

        /// <summary>
        /// Prints a user-readable help text
        /// </summary>
        template <typename TSTREAMT = std::basic_ostream<CHAR>::traits_type>
        inline void PrintHelp(std::basic_ostream<CHAR, TSTREAMT>& stream) const;

        /// <summary>
        /// Prints a user-readable help text
        /// </summary>
        inline void PrintHelp(Command<CHAR> const& command) const;

        /// <summary>
        /// Prints a user-readable help text
        /// </summary>
        inline void PrintHelp(Result& result) const;

        /// <summary>
        /// Prints a user-readable help text
        /// </summary>
        inline void PrintHelp() const;

    private:
        struct StringConsts;

        inline static constexpr size_t cexprStrLen(const CHAR* str) noexcept
        {
            size_t length = 0;
            while (*str++)
            {
                ++length;
            }
            return length;
        }

        template <typename TSTREAMT = std::basic_ostream<CHAR>::traits_type>
        void PrintHelpImpl(Command<CHAR> const* command, std::basic_ostream<CHAR, TSTREAMT>& stream) const;

        class ResultImpl : public Result
        {
        public:
            ResultImpl()
                : Result()
            {
            }
        };

        bool m_withImplicitHelpSwitch = true;
    };

    template <>
    void Parser<char>::Result::PrintError(bool tryUseColor) const
    {
        PrintError(std::cout, tryUseColor);
    }

    template <>
    void Parser<wchar_t>::Result::PrintError(bool tryUseColor) const
    {
        PrintError(std::wcout, tryUseColor);
    }

    template <>
    template <typename TSTREAMT>
    void Parser<char>::Result::PrintError(std::basic_ostream<char, TSTREAMT>& stream, bool tryUseColor) const
    {
        if (Result::m_error.empty())
            return;

        bool useColor = false;
#ifdef _WIN32
        {
            HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD mode;
            if (GetConsoleMode(hStdOut, &mode))
            {
                SetConsoleMode(hStdOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
                if (GetConsoleMode(hStdOut, &mode))
                {
                    if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)
                    {
                        useColor = true;
                    }
                }
            }
        }
#endif
        if (useColor)
            stream << "\x1B[91m\x1B[40m";
        stream << m_error;
        if (useColor)
            stream << "\x1B[0m";
        stream << "\n";
    }

    template <>
    template <typename TSTREAMT>
    void Parser<wchar_t>::Result::PrintError(std::basic_ostream<wchar_t, TSTREAMT>& stream, bool tryUseColor) const
    {
        if (Result::m_error.empty())
            return;
        std::wstringstream wss;
        wss << m_error.c_str();

        bool useColor = false;
#ifdef _WIN32
        {
            HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD mode;
            if (GetConsoleMode(hStdOut, &mode))
            {
                SetConsoleMode(hStdOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
                if (GetConsoleMode(hStdOut, &mode))
                {
                    if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)
                    {
                        useColor = true;
                    }
                }
            }
        }
#endif
        if (useColor)
            stream << "\x1B[91m\x1B[40m";
        stream << wss.str();
        if (useColor)
            stream << "\x1B[0m";
        stream << L"\n";
    }

    template <typename CHAR>
    template <typename TSTREAMT>
    void Parser<CHAR>::PrintHelp(Command<CHAR> const& command, std::basic_ostream<CHAR, TSTREAMT>& stream) const
    {
        PrintHelpImpl(&command, stream);
    }

    template <typename CHAR>
    template <typename TSTREAMT>
    void Parser<CHAR>::PrintHelp(Result& result, std::basic_ostream<CHAR, TSTREAMT>& stream) const
    {
        // TODO: select the deepest command
        PrintHelpImpl(nullptr, stream);
    }

    template <typename CHAR>
    template <typename TSTREAMT>
    void Parser<CHAR>::PrintHelp(std::basic_ostream<CHAR, TSTREAMT>& stream) const
    {
        PrintHelpImpl(nullptr, stream);
    }

    template <>
    void Parser<char>::PrintHelp(Command<char> const& command) const
    {
        PrintHelpImpl(&command, std::cout);
    }

    template <>
    void Parser<char>::PrintHelp(Result& result) const
    {
        PrintHelp(result, std::cout);
    }

    template <>
    void Parser<char>::PrintHelp() const
    {
        PrintHelpImpl(nullptr, std::cout);
    }

    template <>
    void Parser<wchar_t>::PrintHelp(Command<wchar_t> const& command) const
    {
        PrintHelpImpl(&command, std::wcout);
    }

    template <>
    void Parser<wchar_t>::PrintHelp(Result& result) const
    {
        PrintHelp(result, std::wcout);
    }

    template <>
    void Parser<wchar_t>::PrintHelp() const
    {
        PrintHelpImpl(nullptr, std::wcout);
    }

    template <>
    struct Parser<char>::StringConsts
    {
        static constexpr char const nl = '\n';
        static constexpr char const s = ' ';
        static constexpr char const ob = '<';
        static constexpr char const cb = '>';

        static constexpr char const* descriptionCaption = "Description:";
        static constexpr char const* usageCaption = "Usage:";
        static constexpr char const* commandsCaption = "Commands:";
        static constexpr char const* optionsCaption = "Options:";
        static constexpr char const* argumentsCaption = "Arguments:";
        static constexpr char const* command = "[command]";
        static constexpr char const* options = "[options]";

        static constexpr char const* tagRequired = "[required]";
        static constexpr char const* tagOptional = "[optional]";

        static constexpr char const* helpName = "--help";
        static constexpr char const* helpAlias1 = "-h";
        static constexpr char const* helpAlias2 = "/h";
        static constexpr char const* helpAlias3 = "-?";
        static constexpr char const* helpAlias4 = "/?";
        static constexpr char const* helpDescription = "Show help and usage information";
    };

    template <>
    struct Parser<wchar_t>::StringConsts
    {
        static constexpr wchar_t const nl = L'\n';
        static constexpr wchar_t const s = L' ';
        static constexpr wchar_t const ob = L'<';
        static constexpr wchar_t const cb = L'>';

        static constexpr wchar_t const* descriptionCaption = L"Description:";
        static constexpr wchar_t const* usageCaption = L"Usage:";
        static constexpr wchar_t const* commandsCaption = L"Commands:";
        static constexpr wchar_t const* optionsCaption = L"Options:";
        static constexpr wchar_t const* argumentsCaption = L"Arguments:";
        static constexpr wchar_t const* command = L"[command]";
        static constexpr wchar_t const* options = L"[options]";

        static constexpr wchar_t const* tagRequired = L"[required]";
        static constexpr wchar_t const* tagOptional = L"[optional]";

        static constexpr wchar_t const* helpName = L"--help";
        static constexpr wchar_t const* helpAlias1 = L"-h";
        static constexpr wchar_t const* helpAlias2 = L"/h";
        static constexpr wchar_t const* helpAlias3 = L"-?";
        static constexpr wchar_t const* helpAlias4 = L"/?";
        static constexpr wchar_t const* helpDescription = L"Show help and usage information";
    };

    template <typename CHAR>
    std::basic_string<CHAR> WithNameAndAlias<CHAR>::GetAllNames() const
    {
        std::basic_string<CHAR> names;
        for (Alias<CHAR> const& a : m_names)
        {
            if (!names.empty())
            {
                names += static_cast<CHAR>(',');
                names += static_cast<CHAR>(' ');
            }
            names += a.GetName();
        }
        return names;
    }

    template <typename CHAR>
    template <typename TSTREAMT>
    void Parser<CHAR>::PrintHelpImpl(Command<CHAR> const* command, std::basic_ostream<CHAR, TSTREAMT>& stream) const
    {
        using s = StringConsts;
        using string = std::basic_string<CHAR>;
        using stringPair = std::tuple<string, string>;

        int width = 80;
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        {
            width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        }
#else
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        {
            width = w.ws_col;
        }
#endif
        width--; // reserve last column to host an explicit new line

        WithDesciption<CHAR> const* desc = (command == nullptr) ? static_cast<WithDesciption<CHAR> const*>(this)
                                                                : static_cast<WithDesciption<CHAR> const*>(command);
        if (!desc->GetDescription().empty())
        {
            stream << s::descriptionCaption << s::nl;
            typename string::const_iterator descBegin = desc->GetDescription().cbegin();
            typename string::const_iterator descEnd = desc->GetDescription().cend();
            while (descBegin != descEnd)
            {
                size_t descLen = descEnd - descBegin;
                if (descLen > width - 2)
                {
                    descLen = width - 2;
                    for (size_t i = width - 2; i > width / 4; --i)
                    {
                        if (std::isspace(*(descBegin + i)))
                        {
                            descLen = i;
                            break;
                        }
                    }
                }
                stream << s::s << s::s << std::basic_string_view<CHAR>{&*descBegin, descLen} << s::nl;
                descBegin += descLen;
                while (descBegin != descEnd && std::isspace(*descBegin))
                {
                    descBegin++;
                }
            }
            stream << s::nl;
        }

        std::vector<Command<CHAR> const*> commandChain;
        if (command != nullptr)
        {
            std::function<bool(WithCommandContainer<CHAR> const& cmds)> search;
            search = [command, &search, &commandChain](WithCommandContainer<CHAR> const& cmds)
            {
                for (auto cmdIt = cmds.CommandsBegin(); cmdIt != cmds.CommandsEnd(); ++cmdIt)
                {
                    if (WithIdentity<CHAR>::Equals(*command, *cmdIt) ||
                        search(static_cast<WithCommandContainer<CHAR> const&>(*cmdIt)))
                    {
                        commandChain.push_back(&*cmdIt);
                        return true;
                    }
                }
                return false;
            };
            search(static_cast<WithCommandContainer<CHAR> const&>(*this));
        }

        std::vector<Option<CHAR> const*> allOptions;
        std::vector<Switch<CHAR> const*> allSwitches;
        std::vector<Argument<CHAR> const*> allArguments;

        Switch<CHAR> helpSwitch{s::helpName, s::helpDescription};
        helpSwitch.AddAlias(s::helpAlias1).AddAlias(s::helpAlias2).AddAlias(s::helpAlias3).AddAlias(s::helpAlias4);
        if (m_withImplicitHelpSwitch)
        {
            allSwitches.push_back(&helpSwitch);
        }

        auto addRange = [](auto& vec, auto itBegin, auto itEnd)
        {
            for (auto optIt = itBegin; optIt != itEnd; ++optIt)
            {
                vec.push_back(&*optIt);
            }
        };
        addRange(allOptions, Parser<CHAR>::OptionsBegin(), Parser<CHAR>::OptionsEnd());
        addRange(allSwitches, Parser<CHAR>::SwitchesBegin(), Parser<CHAR>::SwitchesEnd());
        addRange(allArguments, Parser<CHAR>::ArgumentsBegin(), Parser<CHAR>::ArgumentsEnd());
        for (auto const& c : commandChain)
        {
            addRange(allOptions, c->OptionsBegin(), c->OptionsEnd());
            addRange(allSwitches, c->SwitchesBegin(), c->SwitchesEnd());
            addRange(allArguments, c->ArgumentsBegin(), c->ArgumentsEnd());
        }

        size_t x = 0;

        stream << s::usageCaption << s::nl << s::s << s::s << Parser<CHAR>::GetName();
        x = 2 + Parser<CHAR>::GetName().size();
        auto optionalLineBreak = [&x, &width, &stream](size_t l)
        {
            if (x + l >= width)
            {
                stream << s::nl << s::s << s::s << s::s;
                x = l + 3;
            }
            else
            {
                x += l;
            }
        };
        for (auto c = commandChain.crbegin(); c != commandChain.crend(); ++c)
        {
            auto const& n = (*c)->NameAliasBegin()->GetName();
            optionalLineBreak(n.size() + 1);
            stream << s::s << n;
        }
        WithCommandContainer<CHAR> const* cmds = (command == nullptr)
                                                     ? static_cast<WithCommandContainer<CHAR> const*>(this)
                                                     : static_cast<WithCommandContainer<CHAR> const*>(command);
        if (cmds->CommandsBegin() != cmds->CommandsEnd())
        {
            optionalLineBreak(cexprStrLen(s::command) + 1);
            stream << s::s << s::command;
        }
        for (Argument<CHAR> const* arg : allArguments)
        {
            if (!arg->IsRequired())
                continue;
            auto const& n = arg->GetName();
            optionalLineBreak(n.size() + 3);
            stream << s::s << s::ob << n << s::cb;
        }
        if (!allOptions.empty() || !allSwitches.empty())
        {
            optionalLineBreak(cexprStrLen(s::options) + 1);
            stream << s::s << s::options;
        }
        stream << s::nl << s::nl;

        std::vector<stringPair> docu;
        auto printAndCountSpaces = [&stream](size_t& counter, size_t count)
        {
            if (count <= 0)
                return;
            stream << std::setfill(s::s) << std::setw(count) << s::s;
            counter += count;
        };
        auto formatDocuTable = [&docu, &width, &stream, &printAndCountSpaces]()
        {
            size_t c1w = 0;
            for (stringPair const& sp : docu)
            {
                size_t s = std::get<0>(sp).size();
                if (c1w < s)
                {
                    c1w = s;
                }
            }
            if (c1w >= width / 4)
            {
                c1w = (width / 4) - 1;
            }
            size_t maxc1w = width / 3;

            for (stringPair const& sp : docu)
            {
                std::vector<std::basic_string_view<CHAR>> nameLines;

                typename string::const_iterator nameBegin = std::get<0>(sp).cbegin();
                typename string::const_iterator nameEnd = std::get<0>(sp).cend();

                while (nameBegin != nameEnd)
                {
                    size_t x = 0;
                    typename string::const_iterator nameNext = nameEnd;

                    size_t nameLen = static_cast<size_t>(nameNext - nameBegin);
                    if (nameLen > c1w)
                    {
                        if (nameLen > maxc1w)
                        {
                            // wrap name text
                            bool found = false;
                            for (typename string::const_iterator it = nameBegin + c1w; it != nameBegin; --it)
                            {
                                if (std::isspace(*it))
                                {
                                    nameNext = it;
                                    found = true;
                                    break;
                                }
                            }

                            if (!found)
                            {
                                for (typename string::const_iterator it = nameBegin + maxc1w; it != nameBegin; --it)
                                {
                                    if (std::isspace(*it))
                                    {
                                        nameNext = it;
                                        found = true;
                                        break;
                                    }
                                }
                            }

                            if (!found)
                            {
                                nameNext = nameBegin + c1w;
                            }
                        }
                        // else, name is only slightly too large. Do hanging text without wrap
                    }

                    nameLines.push_back(
                        std::basic_string_view<CHAR>{&*nameBegin, static_cast<size_t>(nameNext - nameBegin)});

                    while (nameNext != nameEnd && std::isspace(*nameNext))
                    {
                        nameNext++;
                    }
                    nameBegin = nameNext;
                }

                size_t c1ew = c1w;
                for (auto const& s : nameLines)
                {
                    if (c1ew < s.size())
                    {
                        c1ew = s.size();
                    }
                }

                auto nextNameLine = nameLines.cbegin();
                auto descBegin = std::get<1>(sp).cbegin();
                auto descEnd = std::get<1>(sp).cend();
                while (nextNameLine != nameLines.cend() || descBegin != descEnd)
                {
                    size_t x = 0;
                    printAndCountSpaces(x, 2);
                    if (nextNameLine != nameLines.cend())
                    {
                        auto const& name = *nextNameLine;
                        x += name.size();
                        stream << name;
                        nextNameLine++;
                    };
                    printAndCountSpaces(x, c1ew + 2 - x + 2);

                    if (descBegin != descEnd)
                    {

                        typename string::const_iterator descNext = descEnd;
                        size_t descLen = descNext - descBegin;
                        if (descLen > width - x)
                        {
                            descNext = descBegin + (width - x);
                            for (size_t i = width - x; i > (width - x) / 2; --i)
                            {
                                if (std::isspace(*(descBegin + i)))
                                {
                                    descNext = descBegin + i;
                                    break;
                                }
                            }
                        }

                        stream << std::basic_string_view{&*descBegin, static_cast<size_t>(descNext - descBegin)};

                        while (descNext != descEnd && std::isspace(*descNext))
                        {
                            descNext++;
                        }
                        descBegin = descNext;
                    }
                    stream << s::nl;
                }
            }
        };

        if (!allArguments.empty())
        {
            stream << s::argumentsCaption << s::nl;
            docu.clear();
            for (Argument<CHAR> const* arg : allArguments)
            {
                string desc = (arg->IsRequired() ? s::tagRequired : s::tagOptional);
                desc += s::s;
                desc += arg->GetDescription();
                docu.push_back({arg->GetName(), desc});
            }

            formatDocuTable();

            stream << s::nl;
        }

        if (!allOptions.empty() || !allSwitches.empty())
        {
            stream << s::optionsCaption << s::nl;
            docu.clear();
            for (Option<CHAR> const* opt : allOptions)
            {
                docu.push_back({opt->GetAllNames(), opt->GetDescription()});
            }
            for (Switch<CHAR> const* sw : allSwitches)
            {
                docu.push_back({sw->GetAllNames(), sw->GetDescription()});
            }

            formatDocuTable();

            stream << s::nl;
        }

        if (cmds->CommandsBegin() != cmds->CommandsEnd())
        {
            stream << s::commandsCaption << s::nl;
            docu.clear();
            for (auto cmdIt = cmds->CommandsBegin(); cmdIt != cmds->CommandsEnd(); ++cmdIt)
            {
                docu.push_back({cmdIt->GetAllNames(), cmdIt->GetDescription()});
            }

            formatDocuTable();

            stream << s::nl;
        }
    }

    template <typename CHAR>
    Parser<CHAR>::Result Parser<CHAR>::Parse(int argc, const CHAR* const* argv, bool skipFirstArg /* = true */) const
    {
        ResultImpl res{};

        res.SetError("Not Implemented");

        // TODO: Implement

        return res;
    }

} // namespace yaclap

#endif /* _YACLAP_HPP_INCLUDED_ */
