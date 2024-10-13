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

#include <string>
#include <string_view>
#include <vector>

namespace yaclap
{

	template<typename CHAR>
	class Command
	{
	public:
	};

	template<typename CHAR>
	class Argument
	{
	public:
	};

	template<typename CHAR>
	class Option
	{
	public:
		template<typename ST1, typename ST2>
		Option(const ST1& name, const ST2& description, bool isRequired = false)
			: m_description(description), m_isRequired{ isRequired }
		{
			AddAlias(name);
		}
		Option() = default;
		~Option() = default;

		template<typename ST>
		Option& AddAlias(const ST& name)
		{
			m_names.push_back(name);
			return *this;
		}

	private:
		std::vector<std::string> m_names;
		std::string m_description;
		bool m_isRequired;
	};

	template<typename CHAR>
	class Switch
	{
	public:
	};

	template<typename CHAR>
	class Parser
	{
	public:

		class Result
		{
		public:
		};

	};

}

#endif /* _YACLAP_HPP_INCLUDED_ */
