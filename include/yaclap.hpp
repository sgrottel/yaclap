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

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace yaclap
{

	template<typename CHAR>
	class Alias
	{
	public:
		enum class StringCompare {
			CaseSensitive,
			CaseInsensitive
		};

		Alias() noexcept = default;
		Alias(const CHAR* name, StringCompare stringCompare = StringCompare::CaseSensitive)
			: m_name{ name }, m_stringCompare{ stringCompare }
		{}
		template<typename T, typename A>
		Alias(const std::basic_string<CHAR, T, A>& name, StringCompare stringCompare = StringCompare::CaseSensitive)
			: m_name{ name }, m_stringCompare{ stringCompare }
		{}
		template<typename T>
		Alias(const std::basic_string_view<CHAR, T>& name, StringCompare stringCompare = StringCompare::CaseSensitive)
			: m_name{ name }, m_stringCompare{ stringCompare }
		{}

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

	template<typename CHAR>
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
			if (alias.GetName().empty()) throw std::invalid_argument("alias");
			m_names.push_back(alias);
		}

	public:

		std::vector<Alias<CHAR>>::const_iterator NameAliasBegin() const noexcept
		{
			return m_names.cbegin();
		}

		std::vector<Alias<CHAR>>::const_iterator NameAliasEnd() const noexcept
		{
			return m_names.cend();
		}

	private:
		std::vector<Alias<CHAR>> m_names;
	};

	template<typename CHAR>
	class WithDesciption
	{
	protected:
		WithDesciption() noexcept = default;
		template<typename TSTR>
		WithDesciption(const TSTR& description)
			: m_desc{ description }
		{}

	public:

		inline const std::basic_string<CHAR>& GetDescription() const noexcept
		{
			return m_desc;
		}

	private:
		std::basic_string<CHAR> m_desc;
	};

	template<typename CHAR>
	class Argument : public WithDesciption<CHAR>
	{
	public:
		Argument() noexcept = default;
		template<typename TSTR1, typename TSTR2>
		explicit Argument(const TSTR1& name, const TSTR2& description)
			: WithDesciption<CHAR>{ description }, m_name{ name }
		{}

		inline const std::basic_string<CHAR>& GetName() const noexcept
		{
			return m_name;
		}

	private:
		std::basic_string<CHAR> m_name;
	};

	template<typename CHAR>
	class Option : public WithNameAndAlias<CHAR>, public WithDesciption<CHAR>
	{
	public:
		Option() noexcept = default;
		template<typename TSTR1, typename TSTR2>
		explicit Option(const Alias<CHAR>& name, const TSTR1& argumentName, const TSTR2& description)
			: WithNameAndAlias<CHAR>{ name }, WithDesciption<CHAR>{ description }, m_argName{ argumentName }
		{}

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

	template<typename CHAR>
	class Switch : public WithNameAndAlias<CHAR>, public WithDesciption<CHAR>
	{
	public:
		Switch() noexcept = default;
		template<typename TSTR>
		explicit Switch(const Alias<CHAR>& name, const TSTR& description)
			: WithNameAndAlias<CHAR>{ name }, WithDesciption<CHAR>{ description }
		{}

		Switch& AddAlias(const Alias<CHAR>& alias)
		{
			WithNameAndAlias<CHAR>::AddAliasImpl(alias);
			return *this;
		}

	};

	template<typename CHAR>
	class Command;

	template<typename CHAR>
	class WithCommandContainer
	{
	protected:
		WithCommandContainer() noexcept = default;

		void AddCommandImpl(const Command<CHAR>& command);
		void AddOptionImpl(const Option<CHAR>& option)
		{
			if (option.NameAliasBegin() == option.NameAliasEnd()) throw std::invalid_argument("option");
			m_options.push_back(option);
		}
		void AddSwitchImpl(const Switch<CHAR>& switchOption)
		{
			if (switchOption.NameAliasBegin() == switchOption.NameAliasEnd()) throw std::invalid_argument("switchOption");
			m_switches.push_back(switchOption);
		}
		void AddArgumentImpl(const Argument<CHAR>& argument)
		{
			m_arguments.push_back(argument);
		}

	public:

		// TODO: Getters

	private:
		std::vector<Command<CHAR>> m_commands;
		std::vector<Option<CHAR>> m_options;
		std::vector<Switch<CHAR>> m_switches;
		std::vector<Argument<CHAR>> m_arguments;
	};

	template<typename CHAR>
	class Command : public WithCommandContainer<CHAR>, public WithNameAndAlias<CHAR>, public WithDesciption<CHAR>
	{
	public:
		Command() noexcept = default;
		template<typename TSTR>
		Command(const Alias<CHAR>& name, const TSTR& description)
			: WithCommandContainer<CHAR>{}, WithNameAndAlias<CHAR>{ name }, WithDesciption<CHAR>{ description }
		{}

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

	template<typename CHAR>
	void WithCommandContainer<CHAR>::AddCommandImpl(const Command<CHAR>& command)
	{
		if (command.NameAliasBegin() == command.NameAliasEnd()) throw std::invalid_argument("command");
		m_commands.push_back(command);
	}


	template<typename CHAR>
	class Parser : public WithCommandContainer<CHAR>, WithDesciption<CHAR>
	{
	public:
		Parser() noexcept = default;
		template<typename TSTR>
		Parser(const TSTR& description)
			: WithCommandContainer<CHAR>{}, WithDesciption<CHAR>{ description }
		{}

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

		// TODO: Parse and Result

		class Result
		{
		public:
		};

	};

}

#endif /* _YACLAP_HPP_INCLUDED_ */
