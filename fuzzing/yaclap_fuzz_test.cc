
#include "../include/yaclap.hpp"

#include "fuzztest/fuzztest.h"

#include <gtest/gtest.h>

#include <unicode/unistr.h>

#include <cassert>
#include <string>
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

    class StdCoutSilence
    {
    public:
        StdCoutSilence()
            : originalBuffer{std::cout.rdbuf()}, nullStream{}, originalWBuffer{std::wcout.rdbuf()}, nullWStream{}
        {
            std::cout.rdbuf(nullStream.rdbuf());
            std::wcout.rdbuf(nullWStream.rdbuf());
        }

        ~StdCoutSilence()
        {
            try
            {
                std::cout.rdbuf(originalBuffer);
            }
            catch (...)
            {
            }
            try
            {
                std::wcout.rdbuf(originalWBuffer);
            }
            catch (...)
            {
            }
        }
    private:
        std::streambuf* originalBuffer;
        std::ostringstream nullStream;
        std::wstreambuf* originalWBuffer;
        std::wostringstream nullWStream;
    };
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

    inputOption
        .HideFromHelp();

    Command commandA{
        {_T("CommandA").c_str(), StringCompare::CaseInsensitive},
        _T("Command A")};
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

    typename Parser::Result res = parser.Parse(argv.size(), argv.data());

    auto c1 = res.HasCommand(commandA);
    auto c2 = res.HasCommand(commandB);

    auto inputValue = res.GetOptionValue(inputOption, Parser::Result::ErrorIfMultiple);
    if (inputValue)
    {
        auto c3 = inputValue.data();
    }

    for (typename Parser::ResultValueView const& s : res.GetOptionValues(intValueOption))
    {
        auto intVal = s.AsInteger();
        if (intVal.has_value())
        {
            auto c4 = intVal.value();
        }
    }

    auto dValOpt = res.GetOptionValue(doubleValueOption, Parser::Result::ErrorIfMultiple).AsDouble();
    if (dValOpt)
    {
        auto c5 = dValOpt.value();
    }

    auto c6 = res.GetOptionValue(boolValueOption, Parser::Result::ErrorIfMultiple).AsBool().value_or(false);

    auto c7 = static_cast<int>(res.HasSwitch(verboseSwitch));

    auto andValue = res.GetArgument(andArgument);
    if (andValue)
    {
        auto c8 = andValue;
    }

    auto orValue = res.GetArgument(orArgument);
    if (orValue)
    {
        auto c9 = orValue;
    }

    if (res.HasUnmatchedArguments())
    {
        for (auto const& arg : res.UnmatchedArguments())
        {
            auto c10 = arg.data();
        }
    }

//    {
//        StdCoutSilence silence;
//
//        parser.PrintErrorAndHelpIfNeeded(res);
//    }

    auto c11 = res.IsSuccess();
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
        std::vector<std::wstring> wargs;
        wargs.resize(args.size());
        std::transform(args.begin(), args.end(), wargs.begin(), [](std::string const& strU8) -> std::wstring {
            std::wstring wstr;
            Assign(wstr, strU8.c_str());
            return wstr;
        });
        FuzzTestImpl<wchar_t, std::wstring>(wargs, skipFirst);
    }
}

std::vector<std::tuple<std::vector<std::string>, bool, TestStringEncoding>> FuzzTestSeeds()
{
    return {
        {{"yaclap.exe", "cmda", "-i", "whateff.txt", "-v", "-v"}, true, TestStringEncoding::Ascii},
        {{"yaclap.exe", "B",  "/V", "42", "-v", "-v", "-v", "and"}, true, TestStringEncoding::Ascii},
        {{"yaclap.exe", "--help"}, true, TestStringEncoding::Ascii},
        {{"yaclap.exe", "B", "-V"}, true, TestStringEncoding::Ascii},
        {{"cmda", "-i", "whateff.txt", "-v", "-v"}, false, TestStringEncoding::Ascii},
        {{"yaclap.exe", "cmda", "-i", "whateff.txt", "-v", "-v"}, true, TestStringEncoding::Unicode},
        {{"yaclap.exe", "B",  "/V", "42", "-v", "-v", "-v", "and"}, true, TestStringEncoding::Unicode},
        {{"yaclap.exe", "--help"}, true, TestStringEncoding::Unicode},
        {{"yaclap.exe", "B", "-V"}, true, TestStringEncoding::Unicode},
        {{"cmda", "-i", "whateff.txt", "-v", "-v"}, false, TestStringEncoding::Unicode}
    };
}

FUZZ_TEST(Yaclap, FuzzTest)
    .WithDomains(
        fuzztest::VectorOf(fuzztest::String()),
        fuzztest::Arbitrary<bool>(),
        fuzztest::ElementOf<TestStringEncoding>({TestStringEncoding::Ascii, TestStringEncoding::Unicode})
    )
    .WithSeeds(FuzzTestSeeds());
