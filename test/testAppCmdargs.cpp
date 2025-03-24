#include "cmdargs.hpp"

#include <gtest/gtest.h>

#include <string_view>

namespace App
{

    TEST(Cmdargs, CommandA)
    {
        const TCHAR* const argv[] = {_T("yaclap.exe"), _T("cmda"), _T("-i"), _T("whateff.txt"), _T("-v"), _T("-v")};
        const int argc = sizeof(argv) / sizeof(TCHAR*);

        Config cfg;
        bool parseOk = cfg.ParseCmdLine(argc, argv);
        EXPECT_TRUE(parseOk);

        EXPECT_EQ(Command::CommandA, cfg.m_cmd);
        EXPECT_EQ(2, cfg.m_verbose);
        EXPECT_EQ(std::basic_string_view<TCHAR>(_T("whateff.txt")), std::basic_string_view<TCHAR>(cfg.m_input));
        EXPECT_EQ(0, cfg.m_intValue);
        EXPECT_EQ(0.0, cfg.m_doubleValue);
        EXPECT_EQ(false, cfg.m_boolValue);
        EXPECT_EQ(std::basic_string_view<TCHAR>(_T("")), cfg.m_andArg);
    }

    TEST(Cmdargs, CommandB)
    {
        const TCHAR* const argv[] = {_T("yaclap.exe"), _T("B"),  _T("/V"), _T("42"),
                                     _T("-v"),         _T("-v"), _T("-v"), _T("and")};
        const int argc = sizeof(argv) / sizeof(TCHAR*);

        Config cfg;
        bool parseOk = cfg.ParseCmdLine(argc, argv);
        EXPECT_TRUE(parseOk);

        EXPECT_EQ(Command::CommandB, cfg.m_cmd);
        EXPECT_EQ(3, cfg.m_verbose);
        EXPECT_EQ(nullptr, cfg.m_input);
        EXPECT_EQ(42, cfg.m_intValue);
        EXPECT_EQ(0.0, cfg.m_doubleValue);
        EXPECT_EQ(false, cfg.m_boolValue);
        EXPECT_EQ(std::basic_string_view<TCHAR>(_T("and")), cfg.m_andArg);
    }

    namespace
    {
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

    TEST(Cmdargs, Help)
    {
        const TCHAR* const argv[] = {_T("yaclap.exe"), _T("--help")};
        const int argc = sizeof(argv) / sizeof(TCHAR*);

        Config cfg;
        bool parseOk = false;
        {
            StdCoutSilence silence;
            parseOk = cfg.ParseCmdLine(argc, argv);
        }
        EXPECT_TRUE(parseOk);

        EXPECT_EQ(Command::None, cfg.m_cmd);
        EXPECT_EQ(0, cfg.m_verbose);
        EXPECT_EQ(nullptr, cfg.m_input);
        EXPECT_EQ(0, cfg.m_intValue);
        EXPECT_EQ(0.0, cfg.m_doubleValue);
        EXPECT_EQ(false, cfg.m_boolValue);
        EXPECT_EQ(std::basic_string_view<TCHAR>(_T("")), cfg.m_andArg);
    }

    TEST(Cmdargs, Error)
    {
        const TCHAR* const argv[] = {_T("yaclap.exe"),
                                     _T("B"),
                                     _T("-V")};
        const int argc = sizeof(argv) / sizeof(TCHAR*);

        Config cfg;
        bool parseOk = false;
        {
            StdCoutSilence silence;
            parseOk = cfg.ParseCmdLine(argc, argv);
        }
        EXPECT_FALSE(parseOk);
    }

} // namespace App
