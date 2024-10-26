
#include "yaclap.hpp"

#include "cmdargs.hpp"

bool Config::ParseCmdLine(int argc, const _TCHAR* const* argv)
{
    using Parser = yaclap::Parser<_TCHAR>;
    using StringCompare = yaclap::Alias<_TCHAR>::StringCompare;
    using Option = yaclap::Option<_TCHAR>;
    using Switch = yaclap::Switch<_TCHAR>;
    using Argument = yaclap::Argument<_TCHAR>;
    using Command = yaclap::Command<_TCHAR>;

    // clang-format off

    Parser parser{
        _T("yaclap.exe"),
        _T("Example application showing usage of yaclap and used for testing.")};

    Option inputOption{
        {_T("--input"), StringCompare::CaseInsensitive},
        _T("file"),
        _T("An input file")};
    inputOption
        .AddAlias(_T("-i"))
        .AddAlias(_T("/i"));

    Command commandA{
        {_T("CommandA"), StringCompare::CaseInsensitive},
        _T("Command A")};
    commandA
        .AddAlias({_T("CmdA"), StringCompare::CaseInsensitive})
        .AddAlias(_T("A"))
        .Add(inputOption);

    Option valueOption{
        {_T("--value"), StringCompare::CaseInsensitive},
        std::basic_string<_TCHAR>(_T("int")),
        _T("The value option")};
    valueOption
        .AddAlias(_T("-V"))
        .AddAlias(_T("/V"));

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
        .Add(valueOption)
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

    // clang-format on

    Parser::Result res = parser.Parse(argc, argv);

    // mapping parsed command line options to application specific struct:

    // Commands
    if (res.HasCommand(commandA))
        m_cmd = ::Command::CommandA;
    else if (res.HasCommand(commandB))
        m_cmd = ::Command::CommandB;

    // Options

    // TODO: Implement result mapping inputOption

    // TODO: Implement result mapping valueOption

    // Switch
    //  which can be specified multiple times:
    m_verbose = static_cast<int>(res.HasSwitch(verboseSwitch));

    // Arguments
    //  the typical case:
    auto andValue = res.GetArgument(andArgument);
    if (andValue)
        m_andArg = andValue.value();

    //  the special case, with some value computation code:
    auto orValue = res.GetArgument(orArgument);
    if (orValue)
    {
        if (!m_andArg.empty())
            m_andArg += _T(" ");
        m_andArg += _T("| ");
        m_andArg += orValue.value();
    }

    if (res.ShouldShowHelp())
    {
        if (!res.IsSuccess())
        {
            res.PrintError();
            std::cout << "\n";
        }
        parser.PrintHelp(res);
    }

    return res.IsSuccess();
}
