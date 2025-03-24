
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
        const int argc1 = sizeof(argv1) / sizeof(_TCHAR*);

        Parser::Result res = parser.Parse(argc1, argv1);

        EXPECT_TRUE(res.IsSuccess());

        const _TCHAR* const argv2[] = {_T("yaclap.exe"), _T("1"), _T("-o"), _T("2"), _T("3")};
        const int argc2 = sizeof(argv2) / sizeof(_TCHAR*);

        res = parser.Parse(argc2, argv2);

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
        const int argc1 = sizeof(argv1) / sizeof(_TCHAR*);

        Parser::Result res = parser.Parse(argc1, argv1);

        EXPECT_TRUE(res.IsSuccess());
        EXPECT_EQ(2, res.UnmatchedArguments().size());
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("3")), res.UnmatchedArguments()[0]);
        EXPECT_EQ(std::basic_string_view<_TCHAR>(_T("4")), res.UnmatchedArguments()[1]);
    }

} // namespace yaclap_test
