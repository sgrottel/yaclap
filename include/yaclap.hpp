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

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
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

        const std::basic_string<CHAR>& GetName() const noexcept
        {
            return m_name;
        }

        StringCompare GetStringCompareMode() const noexcept
        {
            return m_stringCompare;
        }

        template <typename T>
        bool IsMatch(const std::basic_string_view<CHAR, T>& s) const
        {
            if (s.size() != m_name.size())
                return false;

            if (m_stringCompare == StringCompare::CaseInsensitive)
            {
                for (size_t i = 0; i < s.size(); ++i)
                {
                    if (!AreCharEqualCaseInsenstive(m_name[i], s[i]))
                        return false;
                }
                return true;
            }

            for (size_t i = 0; i < s.size(); ++i)
            {
                if (m_name[i] != s[i])
                    return false;
            }
            return true;
        }

    private:
        std::basic_string<CHAR> m_name;
        StringCompare m_stringCompare;

        static inline bool AreCharEqualCaseInsenstive(CHAR const& a, CHAR const& b);
    };

    template <>
    bool Alias<char>::AreCharEqualCaseInsenstive(char const& a, char const& b)
    {
        return std::tolower(a) == std::tolower(b);
    }

    template <>
    bool Alias<wchar_t>::AreCharEqualCaseInsenstive(wchar_t const& a, wchar_t const& b)
    {
        return std::towlower(a) == std::towlower(b);
    }

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

        template <typename T>
        bool IsMatch(const std::basic_string_view<CHAR, T>& s) const
        {
            for (Alias<CHAR> const& a : m_names)
            {
                if (a.IsMatch(s))
                    return true;
            }
            return false;
        }

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

        template <typename T1, typename T2>
        bool IsMatchWithValue(const std::basic_string_view<CHAR, T1>& s,
                              std::basic_string_view<CHAR, T2>& outValueStr) const
        {
            for (typename std::vector<Alias<CHAR>>::const_iterator a = WithNameAndAlias<CHAR>::NameAliasBegin();
                 a != WithNameAndAlias<CHAR>::NameAliasEnd(); ++a)
            {
                size_t nameLen = a->GetName().size();
                if (nameLen >= s.size())
                    return false;
                // space char is part of the separated, in case option name and value were escaped together as one
                // argument
                if (s[nameLen] != ':' && s[nameLen] != ' ' && s[nameLen] != '=')
                    return false;
                std::basic_string_view<CHAR> sub{s.data(), nameLen};

                if (a->IsMatch(sub))
                {
                    outValueStr = std::basic_string_view<CHAR>{s.data() + nameLen + 1, s.size() - nameLen - 1};
                    return true;
                }
            }

            return false;
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

        inline void SetErrorOnUnmatchedArguments(bool setError = true) noexcept
        {
            m_errorOnUnmatchedArguments = setError;
        }

        inline bool IsSetErrorOnUnmatchedArguments() const noexcept
        {
            return m_errorOnUnmatchedArguments;
        }

        /// <summary>
        /// The parse result only identifies commands, options, switches, and arguments.
        /// Use additional calls on this object to convert and assign values.
        /// </summary>
        class Result
        {
        public:
            using string_t = std::basic_string<CHAR>;
            using string_view_t = std::basic_string_view<CHAR>;

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
            inline void SetError(const CHAR* message, bool setUnsuccessful = true)
            {
                m_error = message;
                if (setUnsuccessful)
                {
                    m_success = false;
                    m_shouldShowHelp = true;
                }
            }

            /// <summary>
            /// Sets the error message to be shown to the user
            /// </summary>
            inline void SetError(const string_t& message, bool setUnsuccessful = true)
            {
                SetError(message.c_str(), setUnsuccessful);
            }

            /// <summary>
            /// Gets the set error message
            /// </summary>
            inline string_t const& GetError() const noexcept
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

            /// <summary>
            /// Returns all Commands occured in the command line in order in which they appeared.
            /// </summary>
            inline std::vector<WithIdentity<CHAR>> const& Commands() const noexcept
            {
                return m_commands;
            }

            /// <summary>
            /// Returns true if the specified Command `cmd` occured in the command line.
            /// </summary>
            inline bool HasCommand(Command<CHAR> const& cmd) const noexcept
            {
                for (auto const& cmdId : m_commands)
                {
                    if (WithIdentity<CHAR>::Equals(cmdId, cmd))
                        return true;
                }
                return false;
            }

            /// <summary>
            /// Returns all Options occured in the command line in order in which they appeared.
            /// </summary>
            inline std::vector<std::tuple<WithIdentity<CHAR>, string_view_t>> const& Options() const noexcept
            {
                return m_options;
            }

            /// <summary>
            /// Returns the number of times the specified option `opt` was seen in the command line (0 = never).
            /// </summary>
            inline size_t GetOptionCount(Option<CHAR> const& opt) const
            {
                return std::count_if(m_options.cbegin(), m_options.cend(),
                                     [&opt](std::tuple<WithIdentity<CHAR>, string_view_t> const& o)
                                     { return WithIdentity<CHAR>::Equals(std::get<0>(o), opt); });
            }

            /// <summary>
            /// Returns the value of the _first_ occurance of the specified Option `opt` in the command line.
            /// </summary>
            inline std::optional<string_view_t> GetOptionValue(Option<CHAR> const& opt) const
            {
                for (std::tuple<WithIdentity<CHAR>, string_view_t> const& o : m_options)
                {
                    if (WithIdentity<CHAR>::Equals(std::get<0>(o), opt))
                    {
                        return std::get<1>(o);
                    }
                }
                return std::nullopt;
            }

            /// <summary>
            /// Alias for 'true', useful as human-readable value for the second parameter of `GetOptionValue`
            /// </summary>
            static constexpr bool ErrorIfMultiple = true;

            /// <summary>
            /// Returns the value of the _first_ occurance of the specified Option `opt` in the command line.
            /// If `setErrorIfMultiple` is set to `ErrorIfMultiple` or `true`, and the Option was found more than one
            /// time in the command line, then an error message is set in the result object, and `nullopt` is returned.
            /// </summary>
            inline std::optional<string_view_t> GetOptionValue(Option<CHAR> const& opt, bool setErrorIfMultiple)
            {
                if (GetOptionCount(opt) > 1)
                {
                    SetError(string_t{StringConsts::errorOptionSpecifiedMultipletimes} +
                             opt.NameAliasBegin()->GetName());
                    return std::nullopt;
                }
                return GetOptionValue(opt);
            }

            /// <summary>
            /// Returns all values of all occurances of the specified Option `opt` in the command line.
            /// </summary>
            inline std::vector<string_view_t> GetOptionValues(Option<CHAR> const& opt) const
            {
                std::vector<string_view_t> result;
                for (std::tuple<WithIdentity<CHAR>, string_view_t> const& o : m_options)
                {
                    if (WithIdentity<CHAR>::Equals(std::get<0>(o), opt))
                    {
                        result.push_back(std::get<1>(o));
                    }
                }
                return result;
            }

            /// <summary>
            /// Returns all Switches occured in the command line in the order in which they appeared
            /// </summary>
            inline std::vector<WithIdentity<CHAR>> const& Switches() const noexcept
            {
                return m_switches;
            }

            /// <summary>
            /// Returns the number of times the specified switch `swt` was seen in the command line (0 = never).
            /// </summary>
            inline size_t HasSwitch(Switch<CHAR> const& swt) const
            {
                return std::count_if(m_switches.cbegin(), m_switches.cend(),
                                     [&swt](WithIdentity<CHAR> const& s)
                                     { return WithIdentity<CHAR>::Equals(s, swt); });
            }

            /// <summary>
            /// Returns all matched Arguments occured in the command line in the order in which they appeared
            /// </summary>
            inline std::vector<std::tuple<WithIdentity<CHAR>, string_view_t>> const& MatchedArguments() const noexcept
            {
                return m_matchedArguments;
            }

            /// <summary>
            /// Gets the value of the specified Argument `arg`
            /// </summary>
            inline std::optional<string_view_t> GetArgument(Argument<CHAR> const& arg) const
            {
                for (auto const& matched : m_matchedArguments)
                {
                    if (WithIdentity<CHAR>::Equals(std::get<0>(matched), arg))
                    {
                        return std::get<1>(matched);
                    }
                }
                return std::nullopt;
            }

            /// <summary>
            /// Returns all unmatched arguments from the command line.
            /// </summary>
            inline std::vector<string_view_t> const& UnmatchedArguments() const noexcept
            {
                return m_unmatchedArguments;
            }

            /// <summary>
            /// Returns true if there is at least one unmatched argument.
            /// </summary>
            inline bool HasUnmatchedArguments() const
            {
                return !m_unmatchedArguments.empty();
            }

        protected:
            Result() = default;

            inline void AddCommand(Command<CHAR> const& cmd)
            {
                m_commands.push_back(cmd);
            }

            template <typename T>
            inline void AddOption(Option<CHAR> const& opt, std::basic_string_view<CHAR, T> const& valueStr)
            {
                m_options.push_back({opt, valueStr});
            }

            inline void AddSwitch(Switch<CHAR> const& swt)
            {
                m_switches.push_back(swt);
            }

            template <typename T>
            inline void AddMatchedArgument(Argument<CHAR> const& arg, std::basic_string_view<CHAR, T> const& valueStr)
            {
                m_matchedArguments.push_back({arg, valueStr});
            }

            template <typename T>
            inline void AddUnmatchedArgument(std::basic_string_view<CHAR, T> const& str)
            {
                m_unmatchedArguments.push_back(str);
            }

            inline void SetShouldShowHelp()
            {
                m_shouldShowHelp = true;
            }

            inline void SetSuccess()
            {
                m_success = true;
            }

        private:
            bool m_success = false;
            bool m_shouldShowHelp = false;
            string_t m_error{};

            std::vector<WithIdentity<CHAR>> m_commands;
            std::vector<std::tuple<WithIdentity<CHAR>, string_view_t>> m_options;
            std::vector<WithIdentity<CHAR>> m_switches;
            std::vector<std::tuple<WithIdentity<CHAR>, string_view_t>> m_matchedArguments;
            std::vector<string_view_t> m_unmatchedArguments;
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
        inline void PrintHelp(Result const& result, std::basic_ostream<CHAR, TSTREAMT>& stream) const;

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
        inline void PrintHelp(Result const& result) const;

        /// <summary>
        /// Prints a user-readable help text
        /// </summary>
        inline void PrintHelp() const;

        /// <summary>
        /// If `result.IsSuccess() == false` prints the error message.
        /// If `result.ShouldShowHelp() == true` prints the usage information.
        /// Both info is printed to `stdout`.
        /// </summary>
        inline void PrintErrorAndHelpIfNeeded(Result const& result) const;

        /// <summary>
        /// If `result.IsSuccess() == false` prints the error message.
        /// If `result.ShouldShowHelp() == true` prints the usage information.
        /// Both info is printed to the specified `stream`.
        /// </summary>
        template <typename TSTREAMT = std::basic_ostream<CHAR>::traits_type>
        inline void PrintErrorAndHelpIfNeeded(Result const& result, std::basic_ostream<CHAR, TSTREAMT>& stream) const;

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

            using Result::AddCommand;
            using Result::AddMatchedArgument;
            using Result::AddOption;
            using Result::AddSwitch;
            using Result::AddUnmatchedArgument;
            using Result::SetShouldShowHelp;
            using Result::SetSuccess;
        };

        bool m_withImplicitHelpSwitch = true;
        bool m_errorOnUnmatchedArguments = true;
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

    template <typename CHAR>
    template <typename TSTREAMT>
    void Parser<CHAR>::Result::PrintError(std::basic_ostream<CHAR, TSTREAMT>& stream, bool tryUseColor) const
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

    template <typename CHAR>
    template <typename TSTREAMT>
    void Parser<CHAR>::PrintHelp(Command<CHAR> const& command, std::basic_ostream<CHAR, TSTREAMT>& stream) const
    {
        PrintHelpImpl(&command, stream);
    }

    template <typename CHAR>
    template <typename TSTREAMT>
    void Parser<CHAR>::PrintHelp(Result const& result, std::basic_ostream<CHAR, TSTREAMT>& stream) const
    {
        Command<CHAR> const* cmd = nullptr;
        if (!result.Commands().empty())
        {
            WithIdentity<CHAR> cmdId = result.Commands().back();
            for (auto cmdIt = WithCommandContainer<CHAR>::CommandsBegin();
                 cmdIt != WithCommandContainer<CHAR>::CommandsEnd(); ++cmdIt)
            {
                if (WithIdentity<CHAR>::Equals(*cmdIt, cmdId))
                {
                    cmd = &*cmdIt;
                    break;
                }
            }
        }
        PrintHelpImpl(cmd, stream);
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
    void Parser<char>::PrintHelp(Result const& result) const
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
    void Parser<wchar_t>::PrintHelp(Result const& result) const
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

        static constexpr char const* errorOptionNoValue = "Value of option expected, but no more arguments: ";
        static constexpr char const* errorUnmatchedArguments = "Unmatched arguments present in command line";
        static constexpr char const* errorRequiredArgumentMissing = "Required argument missing: ";
        static constexpr char const* errorOptionSpecifiedMultipletimes =
            "Option was specified multiple times in the command line: ";

        static inline bool isspace(char c)
        {
            return std::isspace(c);
        }

        static inline char cast(char c)
        {
            return c;
        }
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

        static constexpr wchar_t const* errorOptionNoValue = L"Value of option expected, but no more arguments: ";
        static constexpr wchar_t const* errorUnmatchedArguments = L"Unmatched arguments present in command line";
        static constexpr wchar_t const* errorRequiredArgumentMissing = L"Required argument missing: ";
        static constexpr wchar_t const* errorOptionSpecifiedMultipletimes =
            L"Option was specified multiple times in the command line: ";

        static inline bool isspace(wchar_t c)
        {
            return std::iswspace(c);
        }

        static inline char cast(wchar_t c)
        {
            return (static_cast<int>(c) <= 127) ? static_cast<char>(c) : '?';
        }
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

        size_t width = 80;
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        {
            width = static_cast<size_t>(csbi.srWindow.Right - csbi.srWindow.Left + 1);
        }
#else
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        {
            width = static_cast<size_t>(w.ws_col);
        }
#endif
        if (width < 30)
        {
            width = 30;
        }
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

        auto const addRange = [](auto& vec, auto itBegin, auto itEnd)
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

        Switch<CHAR> helpSwitch{s::helpName, s::helpDescription};
        helpSwitch.AddAlias(s::helpAlias1).AddAlias(s::helpAlias2).AddAlias(s::helpAlias3).AddAlias(s::helpAlias4);
        if (m_withImplicitHelpSwitch)
        {
            allSwitches.push_back(&helpSwitch);
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
                                if (s::isspace(*it))
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
                                    if (s::isspace(*it))
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
                    for (typename string::const_iterator it = nameBegin; it != nameNext; ++it)
                    {
                        if (*it == s::nl)
                        {
                            nameNext = it;
                            break;
                        }
                    }

                    nameLines.push_back(
                        std::basic_string_view<CHAR>{&*nameBegin, static_cast<size_t>(nameNext - nameBegin)});

                    if (nameNext != nameEnd && *nameNext == s::nl)
                    {
                        nameNext++;
                    }
                    else
                    {
                        while (nameNext != nameEnd && std::isspace(*nameNext))
                        {
                            nameNext++;
                        }
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
                docu.push_back({opt->GetAllNames() + s::nl + s::s + s::s + s::ob + opt->GetArgumentName() + s::cb,
                                opt->GetDescription()});
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
        using s = StringConsts;

        ResultImpl res{};

        std::vector<Command<CHAR> const*> allCommands;
        std::vector<Option<CHAR> const*> allOptions;
        std::vector<Switch<CHAR> const*> allSwitches;
        std::vector<Argument<CHAR> const*> allArguments;
        auto const addRange = [](auto& vec, auto itBegin, auto itEnd)
        {
            for (auto optIt = itBegin; optIt != itEnd; ++optIt)
            {
                vec.push_back(&*optIt);
            }
        };

        Switch<CHAR> helpSwitch{s::helpName, s::helpDescription};
        helpSwitch.AddAlias(s::helpAlias1).AddAlias(s::helpAlias2).AddAlias(s::helpAlias3).AddAlias(s::helpAlias4);
        if (m_withImplicitHelpSwitch)
        {
            allSwitches.push_back(&helpSwitch);
        }

        addRange(allCommands, Parser<CHAR>::CommandsBegin(), Parser<CHAR>::CommandsEnd());
        addRange(allOptions, Parser<CHAR>::OptionsBegin(), Parser<CHAR>::OptionsEnd());
        addRange(allSwitches, Parser<CHAR>::SwitchesBegin(), Parser<CHAR>::SwitchesEnd());
        addRange(allArguments, Parser<CHAR>::ArgumentsBegin(), Parser<CHAR>::ArgumentsEnd());

        Option<CHAR> const* pendingOption = nullptr;

        for (int argi = skipFirstArg ? 1 : 0; argi < argc; ++argi)
        {
            const std::basic_string_view<CHAR> arg{argv[argi]};
            bool handled = false;

            if (pendingOption != nullptr)
            {
                res.AddOption(*pendingOption, arg);
                handled = true;
                pendingOption = nullptr;
            }
            if (handled)
                continue;

            for (Command<CHAR> const* cmd : allCommands)
            {
                if (cmd->IsMatch(arg))
                {
                    allCommands.clear();
                    addRange(allCommands, cmd->CommandsBegin(), cmd->CommandsEnd());

                    addRange(allOptions, cmd->OptionsBegin(), cmd->OptionsEnd());
                    addRange(allSwitches, cmd->SwitchesBegin(), cmd->SwitchesEnd());
                    addRange(allArguments, cmd->ArgumentsBegin(), cmd->ArgumentsEnd());

                    res.AddCommand(*cmd);
                    handled = true;
                    break;
                }
            }
            if (handled)
                continue;

            for (Option<CHAR> const* opt : allOptions)
            {
                if (opt->IsMatch(arg))
                {
                    pendingOption = opt;
                    handled = true;
                    break;
                }
                std::basic_string_view<CHAR> valueStr;
                if (opt->IsMatchWithValue(arg, valueStr))
                {
                    handled = true;
                    res.AddOption(*opt, valueStr);
                    break;
                }
            }
            if (handled)
                continue;
            for (Switch<CHAR> const* swt : allSwitches)
            {
                if (swt->IsMatch(arg))
                {
                    handled = true;
                    if (WithIdentity<CHAR>::Equals(*swt, helpSwitch))
                    {
                        res.SetShouldShowHelp();
                    }
                    else
                    {
                        res.AddSwitch(*swt);
                    }
                    break;
                }
            }
            if (handled)
                continue;

            if (!allArguments.empty())
            {
                Argument<CHAR> const* ma = allArguments.front();
                res.AddMatchedArgument(*ma, arg);
                handled = true;
                allArguments.erase(allArguments.begin());
            }
            if (handled)
                continue;

            res.AddUnmatchedArgument(arg);
        }

        Argument<CHAR> const* missingRequiredArgument = nullptr;
        for (Argument<CHAR> const* ma : allArguments)
        {
            if (ma->IsRequired())
            {
                missingRequiredArgument = ma;
                break;
            }
        }

        if (pendingOption != nullptr)
        {
            std::basic_string<CHAR> msg{s::errorOptionNoValue};
            msg += pendingOption->NameAliasBegin()->GetName();
            res.SetError(msg);
        }
        else if (m_errorOnUnmatchedArguments && res.HasUnmatchedArguments())
        {
            res.SetError(s::errorUnmatchedArguments);
        }
        else if (missingRequiredArgument != nullptr)
        {
            std::basic_string<CHAR> msg{s::errorRequiredArgumentMissing};
            msg += missingRequiredArgument->GetName();
            res.SetError(msg);
        }
        else
        {
            res.SetSuccess();
        }

        return res;
    }

    template <>
    void Parser<char>::PrintErrorAndHelpIfNeeded(Result const& result) const
    {
        Parser<char>::PrintErrorAndHelpIfNeeded(result, std::cout);
    }

    template <>
    void Parser<wchar_t>::PrintErrorAndHelpIfNeeded(Result const& result) const
    {
        Parser<wchar_t>::PrintErrorAndHelpIfNeeded(result, std::wcout);
    }

    template <typename CHAR>
    template <typename TSTREAMT>
    void Parser<CHAR>::PrintErrorAndHelpIfNeeded(Result const& result, std::basic_ostream<CHAR, TSTREAMT>& stream) const
    {
        if (!result.IsSuccess())
        {
            result.PrintError();
            stream << StringConsts::nl;
        }
        if (result.ShouldShowHelp())
        {
            Parser<CHAR>::PrintHelp(result);
        }
    }

} // namespace yaclap

#endif /* _YACLAP_HPP_INCLUDED_ */
