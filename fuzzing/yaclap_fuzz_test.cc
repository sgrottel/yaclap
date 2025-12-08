
#include "../include/yaclap.hpp"

#include "fuzztest/fuzztest.h"

#include <gtest/gtest.h>

#include <unicode/unistr.h>

#include <cassert>
#include <string>
#include <sstream>
#include <vector>

namespace
{

    void Assign(std::string& os, const char* is)
    {
        os = is;
    }

    void Assign (std::wstring& os, const char* is)
    {
        icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(is);
        os.resize(ustr.length());
        for (int i = 0; i < ustr.length(); ++i) {
            os[i] = static_cast<wchar_t>(ustr[i]);
        }
    }
}


template<typename CHAR, typename STR>
void FuzzTestImpl(const std::vector<STR>& args, bool skipFirst)
{
    constexpr auto _T = [](const char* str) -> STR {
        STR os;
        Assign(os, str);
        return os;
    };

    using Parser = yaclap::Parser<CHAR>;
    using StringCompare = typename yaclap::Alias<CHAR>::StringCompare;
    using Option = yaclap::Option<CHAR>;
    using Switch = yaclap::Switch<CHAR>;
    using Argument = yaclap::Argument<CHAR>;
    using Command = yaclap::Command<CHAR>;

    Parser parser{_T("yaclap-fuzzing"), _T("Fuzz tests for fun and profit.")};

    parser.SetErrorOnUnmatchedArguments(false);

    Option inputOption{
        {_T("--input"), StringCompare::CaseInsensitive},
        _T("file"),
        _T("An input file")};

    inputOption
        .AddAlias(_T("-i").c_str())
        .AddAlias(_T("/i"));
    try
    {
        inputOption.AddAlias(_T(""));
    }
    catch (...)
    {
    }

    inputOption
        .HideFromHelp();

    Command commandA{
        {_T("CommandA").c_str(), StringCompare::CaseInsensitive},
        _T("Command A description with a very long string to hopefully force a line break when printing the help text into the string stream"
           " down below at the end of the fuzz test. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed eiusmod tempor incidunt ut"
           " labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi"
           " consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat"
           " cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum")};
    commandA
        .AddAlias({_T("CmdA"), StringCompare::CaseInsensitive})
        .AddAlias(_T("A"))
        .Add(inputOption);

    Option intValueOption{
        {_T("--value"), StringCompare::CaseInsensitive},
        STR(_T("int")),
        _T("The value option is an int. If specified multiple times, the values will be summarized.")};
    intValueOption
        .AddAlias(_T("-V"))
        .AddAlias(_T("/V"));

    Option doubleValueOption{_T("--double"), _T("dval"), _T("A double-precision float value. Must not be specified more than once.")};

    Option boolValueOption{_T("--bool"), _T("bval"), _T("A boolean value. Must not be specified more than once.")};

    Argument andArgument{
        _T("and"),
        _T("An additional string argument")};

    Argument orArgument{
        _T("or"),
        _T("An optional string argument"),
        Argument::NotRequired};

    Command commandB{
        {_T("CommandB"), StringCompare::CaseInsensitive},
        _T("Command B")};
    commandB
        .AddAlias({_T("CmdB"), StringCompare::CaseInsensitive})
        .AddAlias(_T("B"))
        .Add(intValueOption)
        .Add(doubleValueOption)
        .Add(boolValueOption)
        .Add(andArgument)
        .Add(orArgument);

    Switch verboseSwitch{
        {_T("--verbose"), StringCompare::CaseInsensitive},
        _T("Verbosity switch")};
    verboseSwitch
        .AddAlias(_T("-v"))
        .AddAlias(_T("/v"));

    parser
        .Add(commandA)
        .Add(commandB)
        .Add(verboseSwitch);

    std::vector<const CHAR*> argv;
    argv.resize(args.size());
    std::transform(args.begin(), args.end(), argv.begin(), [](auto const& a) { return a.c_str(); });

    typename Parser::Result res = parser.Parse(argv.size(), argv.data(), skipFirst);

    auto c1 = res.HasCommand(commandA);
    auto c2 = res.HasCommand(commandB);
    auto c3 = res.Commands();
    auto c4 = res.ShouldShowHelp();

    auto inputValue = res.GetOptionValue(inputOption, Parser::Result::ErrorIfMultiple);
    if (inputValue)
    {
        auto c5 = inputValue.data();
    }

    for (typename Parser::ResultValueView const& s : res.GetOptionValues(intValueOption))
    {
        auto intVal = s.AsInteger();
        if (intVal.has_value())
        {
            auto c6 = intVal.value();
        }
    }

    auto dValOpt = res.GetOptionValue(doubleValueOption, Parser::Result::ErrorIfMultiple).AsDouble();
    if (dValOpt)
    {
        auto c7 = dValOpt.value();
    }

    auto c8 = res.GetOptionValue(boolValueOption, Parser::Result::ErrorIfMultiple).AsBool().value_or(false);

    auto c9 = static_cast<int>(res.HasSwitch(verboseSwitch));

    auto andValue = res.GetArgument(andArgument);
    if (andValue)
    {
        auto c10 = andValue;
    }

    auto orValue = res.GetArgument(orArgument);
    if (orValue)
    {
        auto c11 = orValue;
    }

    if (res.HasUnmatchedArguments())
    {
        for (auto const& arg : res.UnmatchedArguments())
        {
            auto c12 = arg.data();
        }
    }

    {
        std::basic_stringstream<CHAR, std::char_traits<CHAR>, std::allocator<CHAR>> mem;
        res.PrintError(mem, false);
        parser.PrintHelp(mem);
        parser.PrintHelp(res, mem);
        parser.PrintHelp(commandA, mem);
        parser.PrintHelp(commandB, mem);
        parser.PrintErrorAndHelpIfNeeded(res, mem);
    }

    auto c13 = res.IsSuccess();

    auto c14 = doubleValueOption.GetArgumentName();

    res.SetError(_T("").c_str(), false);
}

enum class TestStringEncoding {
    Ascii,
    Unicode
};

void FuzzTest(const std::vector<std::string>& args, bool skipFirst, TestStringEncoding encoding)
{
    if (encoding == TestStringEncoding::Ascii)
    {
        FuzzTestImpl<char, std::string>(args, skipFirst);
    }
    else 
    {
        assert(encoding == TestStringEncoding::Unicode);
        std::vector<std::wstring> wargs(args.size(), L"");
        std::transform(args.begin(), args.end(), wargs.begin(), [](std::string const& strU8) -> std::wstring {
            std::wstring wstr;
            Assign(wstr, strU8.c_str());
            return wstr;
        });
        FuzzTestImpl<wchar_t, std::wstring>(wargs, skipFirst);
    }
}

using TestInputData = std::tuple<std::vector<std::string>, bool, TestStringEncoding>;

std::vector<TestInputData> FuzzTestSeeds()
{
    std::vector<TestInputData> data;

    data.push_back({{"yaclap.exe", "cmda", "-i", "whateff.txt", "-v", "-v"}, true, TestStringEncoding::Ascii});
    data.push_back({{"yaclap.exe", "B", "/V", "42", "-v", "-v", "-v", "and"}, true, TestStringEncoding::Ascii});
    data.push_back({{"yaclap.exe", "--help"}, true, TestStringEncoding::Ascii});
    data.push_back({{"yaclap.exe", "B", "-V"}, true, TestStringEncoding::Ascii});
    data.push_back({{"CommandA", "--input", "whateff.txt", "-v", "/v"}, false, TestStringEncoding::Ascii});
    data.push_back({{"A", "/i", "whateff.txt", "-v", "/v"}, false, TestStringEncoding::Ascii});
    data.push_back({{"CommandB", "--double", "3.74", "and"}, false, TestStringEncoding::Ascii});
    data.push_back({{"CmdB", "--double", "-1.374E-1", "and"}, false, TestStringEncoding::Ascii});
    data.push_back({{"CmdB", "--double", "+2.374e+2", "and"}, false, TestStringEncoding::Ascii});
    data.push_back({{"B", "--double", "no", "and"}, false, TestStringEncoding::Ascii});
    data.push_back({{"B", "--double", "", "and"}, false, TestStringEncoding::Ascii});
    data.push_back({{"CommandB", "--bool", "true", "and"}, false, TestStringEncoding::Ascii});
    data.push_back({{"CmdB", "--bool", "no", "and"}, false, TestStringEncoding::Ascii});
    data.push_back({{"CmdB", "--bool", "no", "and", "or"}, false, TestStringEncoding::Ascii});
    data.push_back({{"CmdB", "--bool", "no", "and", "or", "else"}, false, TestStringEncoding::Ascii});
    data.push_back({{"CmdB", "--bool", "1", "and"}, false, TestStringEncoding::Ascii});
    data.push_back({{"CmdB", "--bool", "1"}, false, TestStringEncoding::Ascii});
    data.push_back({{"B", "--bool", "", "and"}, false, TestStringEncoding::Ascii});
    data.push_back({{"B", "--bool", "idontthinkso", "and"}, false, TestStringEncoding::Ascii});
    data.push_back({{"yaclap.exe", "B", "-V", "+xafFE0123456789", "and"}, true, TestStringEncoding::Ascii});
    data.push_back({{"yaclap.exe", "B", "-V", "-o777", "and"}, true, TestStringEncoding::Ascii});
    data.push_back({{"yaclap.exe", "B", "-V", "-b0110", "and"}, true, TestStringEncoding::Ascii});
    data.push_back({{"yaclap.exe", "B", "-V", "-bG", "and"}, true, TestStringEncoding::Ascii});
    data.push_back({{"yaclap.exe", "B", "-V", "+H", "and"}, true, TestStringEncoding::Ascii});
    data.push_back({{"yaclap.exe", "B", "-V", "Nonono", "and"}, true, TestStringEncoding::Ascii});
    data.push_back({{"yaclap.exe", "B", "-V", "Nonono"}, true, TestStringEncoding::Ascii});
    data.push_back({{"B", "--bool", "true", "--bool", "false", "and"}, false, TestStringEncoding::Ascii});

    // duplicate input cases for unicode
    const size_t cnt = data.size();
    for (size_t i = 0; i < cnt; ++i)
    {
        auto di = data[i];
        data.push_back({std::get<0>(di), std::get<1>(di), TestStringEncoding::Unicode});
    }

    data.push_back({{"yaclap.exe", "cmda", "-i", u8"破滅"}, true, TestStringEncoding::Unicode});

    return data;
}

FUZZ_TEST(Yaclap, FuzzTest)
    .WithDomains(
        fuzztest::VectorOf(fuzztest::String()),
        fuzztest::Arbitrary<bool>(),
        fuzztest::ElementOf<TestStringEncoding>({TestStringEncoding::Ascii, TestStringEncoding::Unicode})
    )
    .WithSeeds(FuzzTestSeeds());
