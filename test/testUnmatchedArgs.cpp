
#include "yaclap.hpp"

#ifdef _WIN32
#include <tchar.h>
#else
#define _TCHAR char
#define _T(A) A
#endif

#include <gtest/gtest.h>

namespace yaclap_test
{

    TEST(UnmatchedArgs, DisallowedByDefault)
    {
        using Parser = yaclap::Parser<_TCHAR>;
        //using StringCompare = yaclap::Alias<_TCHAR>::StringCompare;
        using Option = yaclap::Option<_TCHAR>;
        using Argument = yaclap::Argument<_TCHAR>;

        Parser parser{_T("yaclap.exe"), _T("desc.")};
        Argument arg{_T("a"), _T("desc.")};
        Option opt{_T("-o"), _T("o"), _T("desc.")};
        parser.Add(arg).Add(opt);

        const _TCHAR* const argv1[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2")};
        Parser::Result res = parser.Parse(sizeof(argv1) / sizeof(_TCHAR*), argv1);

        EXPECT_TRUE(res.IsSuccess());

        const _TCHAR* const argv2[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2"), _T("3")};
        res = parser.Parse(sizeof(argv2) / sizeof(_TCHAR*), argv2);

        EXPECT_FALSE(res.IsSuccess());
    }

    TEST(UnmatchedArgs, AllowedAndCaptured)
    {
        using Parser = yaclap::Parser<_TCHAR>;
        // using StringCompare = yaclap::Alias<_TCHAR>::StringCompare;
        using Option = yaclap::Option<_TCHAR>;
        using Argument = yaclap::Argument<_TCHAR>;

        Parser parser{_T("yaclap.exe"), _T("desc.")};
        Argument arg{_T("a"), _T("desc.")};
        Option opt{_T("-o"), _T("o"), _T("desc.")};
        parser.Add(arg).Add(opt);
        parser.SetErrorOnUnmatchedArguments(false);

        const _TCHAR* const argv1[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2"), _T("3"), _T("4")};
        Parser::Result res = parser.Parse(sizeof(argv1) / sizeof(_TCHAR*), argv1);

        EXPECT_TRUE(res.IsSuccess());
        EXPECT_EQ(2, res.UnmatchedArguments().size());
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("3")), res.UnmatchedArguments()[0]);
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("4")), res.UnmatchedArguments()[1]);
    }

    TEST(UnmatchedArgs, DisallowedAndSubDisallowed)
    {
        using Parser = yaclap::Parser<_TCHAR>;
        using Command = yaclap::Command<_TCHAR>;
        using Option = yaclap::Option<_TCHAR>;
        using Argument = yaclap::Argument<_TCHAR>;

        Parser parser{_T("yaclap.exe"), _T("desc.")};
        Argument arg{_T("a"), _T("desc.")};
        Option opt{_T("-o"), _T("o"), _T("desc.")};
        Command cmd{_T("cmd"), _T("desc.")};
        cmd.SetErrorOnUnmatchedArguments(Command::OnUnmatchedArguments::SetError);
        parser.Add(arg).Add(opt).Add(cmd);

        EXPECT_EQ(Command::OnUnmatchedArguments::SetError, cmd.GetSetErrorOnUnmatchedArguments());

        const _TCHAR* const argv1[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2")};
        Parser::Result res = parser.Parse(sizeof(argv1) / sizeof(_TCHAR*), argv1);

        EXPECT_TRUE(res.IsSuccess());

        const _TCHAR* const argv2[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2"), _T("3")};
        res = parser.Parse(sizeof(argv2) / sizeof(_TCHAR*), argv2);

        EXPECT_FALSE(res.IsSuccess());

        const _TCHAR* const argv3[] = {_T("yaclap.exe"), _T("cmd"), _T("1"), _T("-o"), _T("2")};
        res = parser.Parse(sizeof(argv3) / sizeof(_TCHAR*), argv3);

        EXPECT_TRUE(res.IsSuccess());

        const _TCHAR* const argv4[] = {_T("yaclap.exe"), _T("cmd"), _T("1"), _T("-o"), _T("2"), _T("3")};
        res = parser.Parse(sizeof(argv4) / sizeof(_TCHAR*), argv4);

        EXPECT_FALSE(res.IsSuccess());
    }

    TEST(UnmatchedArgs, DisallowedAndSubDefault)
    {
        using Parser = yaclap::Parser<_TCHAR>;
        using Command = yaclap::Command<_TCHAR>;
        using Option = yaclap::Option<_TCHAR>;
        using Argument = yaclap::Argument<_TCHAR>;

        Parser parser{_T("yaclap.exe"), _T("desc.")};
        Argument arg{_T("a"), _T("desc.")};
        Option opt{_T("-o"), _T("o"), _T("desc.")};
        Command cmd{_T("cmd"), _T("desc.")};
        parser.Add(arg).Add(opt).Add(cmd);

        EXPECT_EQ(Command::OnUnmatchedArguments::Keep, cmd.GetSetErrorOnUnmatchedArguments());

        const _TCHAR* const argv1[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2")};
        Parser::Result res = parser.Parse(sizeof(argv1) / sizeof(_TCHAR*), argv1);

        EXPECT_TRUE(res.IsSuccess());

        const _TCHAR* const argv2[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2"), _T("3")};
        res = parser.Parse(sizeof(argv2) / sizeof(_TCHAR*), argv2);

        EXPECT_FALSE(res.IsSuccess());

        const _TCHAR* const argv3[] = {_T("yaclap.exe"), _T("cmd"), _T("1"), _T("-o"), _T("2")};
        res = parser.Parse(sizeof(argv3) / sizeof(_TCHAR*), argv3);

        EXPECT_TRUE(res.IsSuccess());

        const _TCHAR* const argv4[] = {_T("yaclap.exe"), _T("cmd"), _T("1"), _T("-o"), _T("2"), _T("3")};
        res = parser.Parse(sizeof(argv4) / sizeof(_TCHAR*), argv4);

        EXPECT_FALSE(res.IsSuccess());
    }

    TEST(UnmatchedArgs, AllowedAndSubDefaultAndCaptured)
    {
        using Parser = yaclap::Parser<_TCHAR>;
        using Command = yaclap::Command<_TCHAR>;
        using Option = yaclap::Option<_TCHAR>;
        using Argument = yaclap::Argument<_TCHAR>;

        Parser parser{_T("yaclap.exe"), _T("desc.")};
        Argument arg{_T("a"), _T("desc.")};
        Option opt{_T("-o"), _T("o"), _T("desc.")};
        Command cmd{_T("cmd"), _T("desc.")};
        parser.Add(arg).Add(opt).Add(cmd);
        parser.SetErrorOnUnmatchedArguments(false);

        EXPECT_EQ(Command::OnUnmatchedArguments::Keep, cmd.GetSetErrorOnUnmatchedArguments());

        const _TCHAR* const argv1[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2"), _T("3"), _T("4")};
        Parser::Result res = parser.Parse(sizeof(argv1) / sizeof(_TCHAR*), argv1);

        EXPECT_TRUE(res.IsSuccess());
        EXPECT_EQ(2, res.UnmatchedArguments().size());
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("3")), res.UnmatchedArguments()[0]);
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("4")), res.UnmatchedArguments()[1]);

        const _TCHAR* const argv2[] = {_T("yaclap.exe"), _T("cmd"), _T("1"), _T("-o"), _T("2"), _T("3"), _T("4")};
        res = parser.Parse(sizeof(argv2) / sizeof(_TCHAR*), argv2);

        EXPECT_TRUE(res.IsSuccess());
        EXPECT_EQ(2, res.UnmatchedArguments().size());
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("3")), res.UnmatchedArguments()[0]);
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("4")), res.UnmatchedArguments()[1]);
    }

    TEST(UnmatchedArgs, AllowedAndSubAllowedAndCaptured)
    {
        using Parser = yaclap::Parser<_TCHAR>;
        using Command = yaclap::Command<_TCHAR>;
        using Option = yaclap::Option<_TCHAR>;
        using Argument = yaclap::Argument<_TCHAR>;

        Parser parser{_T("yaclap.exe"), _T("desc.")};
        Argument arg{_T("a"), _T("desc.")};
        Option opt{_T("-o"), _T("o"), _T("desc.")};
        Command cmd{_T("cmd"), _T("desc.")};
        cmd.SetErrorOnUnmatchedArguments(Command::OnUnmatchedArguments::NoError);
        parser.Add(arg).Add(opt).Add(cmd);
        parser.SetErrorOnUnmatchedArguments(false);

        EXPECT_EQ(Command::OnUnmatchedArguments::NoError, cmd.GetSetErrorOnUnmatchedArguments());

        const _TCHAR* const argv1[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2"), _T("3"), _T("4")};
        Parser::Result res = parser.Parse(sizeof(argv1) / sizeof(_TCHAR*), argv1);

        EXPECT_TRUE(res.IsSuccess());
        EXPECT_EQ(2, res.UnmatchedArguments().size());
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("3")), res.UnmatchedArguments()[0]);
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("4")), res.UnmatchedArguments()[1]);

        const _TCHAR* const argv2[] = {_T("yaclap.exe"), _T("cmd"), _T("1"), _T("-o"), _T("2"), _T("3"), _T("4")};
        res = parser.Parse(sizeof(argv2) / sizeof(_TCHAR*), argv2);

        EXPECT_TRUE(res.IsSuccess());
        EXPECT_EQ(2, res.UnmatchedArguments().size());
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("3")), res.UnmatchedArguments()[0]);
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("4")), res.UnmatchedArguments()[1]);
    }

    TEST(UnmatchedArgs, DisallowedAndSubAllowedAndCaptured)
    {
        using Parser = yaclap::Parser<_TCHAR>;
        using Command = yaclap::Command<_TCHAR>;
        using Option = yaclap::Option<_TCHAR>;
        using Argument = yaclap::Argument<_TCHAR>;

        Parser parser{_T("yaclap.exe"), _T("desc.")};
        Argument arg{_T("a"), _T("desc.")};
        Option opt{_T("-o"), _T("o"), _T("desc.")};
        Command cmd{_T("cmd"), _T("desc.")};
        cmd.SetErrorOnUnmatchedArguments(Command::OnUnmatchedArguments::NoError);
        parser.Add(arg).Add(opt).Add(cmd);

        EXPECT_EQ(Command::OnUnmatchedArguments::NoError, cmd.GetSetErrorOnUnmatchedArguments());

        const _TCHAR* const argv1[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2")};
        Parser::Result res = parser.Parse(sizeof(argv1) / sizeof(_TCHAR*), argv1);

        EXPECT_TRUE(res.IsSuccess());

        const _TCHAR* const argv2[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2"), _T("3")};
        res = parser.Parse(sizeof(argv2) / sizeof(_TCHAR*), argv2);

        EXPECT_FALSE(res.IsSuccess());

        const _TCHAR* const argv3[] = {_T("yaclap.exe"), _T("cmd"), _T("1"), _T("-o"), _T("2")};
        res = parser.Parse(sizeof(argv3) / sizeof(_TCHAR*), argv3);

        EXPECT_TRUE(res.IsSuccess());
        EXPECT_EQ(0, res.UnmatchedArguments().size());

        const _TCHAR* const argv4[] = {_T("yaclap.exe"), _T("cmd"), _T("1"), _T("-o"), _T("2"), _T("3"), _T("4")};
        res = parser.Parse(sizeof(argv4) / sizeof(_TCHAR*), argv4);

        EXPECT_TRUE(res.IsSuccess());
        EXPECT_EQ(2, res.UnmatchedArguments().size());
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("3")), res.UnmatchedArguments()[0]);
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("4")), res.UnmatchedArguments()[1]);
    }

    TEST(UnmatchedArgs, AllowedAndSubDisallowed)
    {
        using Parser = yaclap::Parser<_TCHAR>;
        using Command = yaclap::Command<_TCHAR>;
        using Option = yaclap::Option<_TCHAR>;
        using Argument = yaclap::Argument<_TCHAR>;

        Parser parser{_T("yaclap.exe"), _T("desc.")};
        Argument arg{_T("a"), _T("desc.")};
        Option opt{_T("-o"), _T("o"), _T("desc.")};
        Command cmd{_T("cmd"), _T("desc.")};
        cmd.SetErrorOnUnmatchedArguments(Command::OnUnmatchedArguments::SetError);
        parser.Add(arg).Add(opt).Add(cmd);
        parser.SetErrorOnUnmatchedArguments(false);

        EXPECT_EQ(Command::OnUnmatchedArguments::SetError, cmd.GetSetErrorOnUnmatchedArguments());

        const _TCHAR* const argv1[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2"), _T("3"), _T("4")};
        Parser::Result res = parser.Parse(sizeof(argv1) / sizeof(_TCHAR*), argv1);

        EXPECT_TRUE(res.IsSuccess());
        EXPECT_EQ(2, res.UnmatchedArguments().size());
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("3")), res.UnmatchedArguments()[0]);
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("4")), res.UnmatchedArguments()[1]);

        const _TCHAR* const argv2[] = {_T("yaclap.exe"), _T("cmd"), _T("1"), _T("-o"), _T("2"), _T("3"), _T("4")};
        res = parser.Parse(sizeof(argv2) / sizeof(_TCHAR*), argv2);

        EXPECT_FALSE(res.IsSuccess());
    }

} // namespace yaclap_test
