
#include "yaclap.hpp"

#include "cmdargs.hpp"

#include <iostream>

bool Config::ParseCmdLine(int argc, const _TCHAR* const* argv)
{
    using Parser = yaclap::Parser<_TCHAR>;
    using StringCompare = yaclap::Alias<_TCHAR>::StringCompare;
    using Option = yaclap::Option<_TCHAR>;
    using Switch = yaclap::Switch<_TCHAR>;
    using Argument = yaclap::Argument<_TCHAR>;
    using Command = yaclap::Command<_TCHAR>;

    // clang-format off

    // These lines specify the command line syntax

    // The entry point is the `Parser` object, representing our application
    // Every object has a description string, which is used to generate a help text output. Be precice and helpful to
    // the users of your software.
    Parser parser{
        _T("yaclap.exe"),
        _T("Example application showing usage of yaclap and used for testing.")};

    // usually, you want to keep this on true (default), unless you explicitly want to do something with the unmatched arguments
    parser.SetErrorOnUnmatchedArguments(false);

    // Options with their values are usually optional
    // Input example:  --input C:\path\file.ext
    Option inputOption{
        {_T("--input"), StringCompare::CaseInsensitive},
        _T("file"),
        _T("An input file")};

    // Alias allow for alternative or shorter input.
    // Input example:  -i C:\path\file.ext
    inputOption
        .AddAlias(_T("-i"))
        .AddAlias(_T("/i"));

    // Do not show this option in the generated help text
    inputOption
        .HideFromHelp();

    // A Command changes the 'scope' of parsing, by adding further Options, Switches, and Arguments to the pool of
    // objects from which the input command line will be matched. You can use Commands to structure your application's
    // behavior and to limit the scope of Options, Switches, and Arguments, i.e., instead of ignoring Options, Switches,
    // and Arguments not applicable in a specific mode of your application, organize these modes with Commands and only
    // add Options, Switches, and Arguments to the Commands, and thus parsing scopes, they apply to.
    Command commandA{
        {_T("CommandA"), StringCompare::CaseInsensitive},
        _T("Command A")};
    commandA
        .AddAlias({_T("CmdA"), StringCompare::CaseInsensitive})
        .AddAlias(_T("A"))
        .Add(inputOption);

    Option intValueOption{
        {_T("--value"), StringCompare::CaseInsensitive},
        std::basic_string<_TCHAR>(_T("int")),
        _T("The value option is an int. If specified multiple times, the values will be summarized.")};
    intValueOption
        .AddAlias(_T("-V"))
        .AddAlias(_T("/V"));

    Option doubleValueOption{_T("--double"), _T("dval"), _T("A double-precision float value. Must not be specified more than once.")};

    Option boolValueOption{_T("--bool"), _T("bval"), _T("A boolean value. Must not be specified more than once.")};

    // An Argument is a named placeholder for a command line argument not matched otherwise as Command, Option, or
    // Switch. These are usually required input for specific commands.
    Argument andArgument{
        _T("and"),
        _T("An additional string argument")};

    // An Argument can be marked as optional. This is a rare edge case. Consider using a Option instead!
    //
    // By default Arguments are required. If an required argument is missing when parsig a command line input this will
    // set an error message and the parsing result will not be a success. Arguments are matched in order, regardless
    // whether they are required or optional. If you specify an optional Argument and afterwards a required Argument,
    // then the first Argument will be effectively required as well, because the second, the explicitly required
    // Argument will only be matched after the first Argument was matched as well.
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

    // A Switch can be understood as special case of an Option. A Switch has no value. It either occurs in the command
    // line input, one or more times, or it does not.
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

    // Invoke the specified parser with the command line input, and get the result.
    Parser::Result res = parser.Parse(argc, argv);

    // Now, we map parsed command line result to our application specific struct. This separates the command line
    // parsing code from the rest of our application code, avoids leakage of types (templates), and any kind of
    // specialized logic, and simplifies the usage of the results in the rest of the application.

    // Commands
    if (res.HasCommand(commandA))
    {
        m_cmd = ::Command::CommandA;
    }
    else if (res.HasCommand(commandB))
    {
        m_cmd = ::Command::CommandB;
    }

    // Options
    auto inputValue = res.GetOptionValue(inputOption, Parser::Result::ErrorIfMultiple);
    if (inputValue)
    {
        // This assignment is ok.
        // Because the input strings are always handled as string_views, the pointer returned from
        // `inputValue.data()` here is a pointer into the original command line argument strings.
        m_input = inputValue.data();
    }

    m_intValue = 0;
    // if multiple occurances are allowed, iterate through all ...
    for (Parser::ResultValueView const& s : res.GetOptionValues(intValueOption))
    {
        // ... cast to type if you need to, which will also set an error message if it fails, ...
        auto intVal = s.AsInteger();
        // ... and if the optional has a value ...
        if (intVal.has_value())
        {
            // ... handle it, e.g. by adding.
            m_intValue += intVal.value();
        }
    }

    // Or, if only one occurance is allowed, fetch that value (or set an error), cast if needed, and check the returned
    // optional, and assign it's value if present
    auto dValOpt = res.GetOptionValue(doubleValueOption, Parser::Result::ErrorIfMultiple).AsDouble();
    if (dValOpt)
    {
        m_doubleValue = dValOpt.value();
    }

    // ... or always assign value of optional or default.
    m_boolValue = res.GetOptionValue(boolValueOption, Parser::Result::ErrorIfMultiple).AsBool().value_or(false);

    // Switch
    // which can be specified multiple times:
    m_verbose = static_cast<int>(res.HasSwitch(verboseSwitch));

    // Arguments
    // the typical case:
    auto andValue = res.GetArgument(andArgument);
    if (andValue)
    {
        m_andArg = andValue;
    }

    // the special case, with some value computation code:
    auto orValue = res.GetArgument(orArgument);
    if (orValue)
    {
        if (!m_andArg.empty())
        {
            m_andArg += _T(" ");
        }
        m_andArg += _T("| ");
        m_andArg += orValue;
    }

    // In this test application, we report unmatched arguments:
    if (res.HasUnmatchedArguments())
    {
        std::wcout << L"Unmatched arguments: " << res.UnmatchedArguments().size() << L"\n";
        for (auto const& arg : res.UnmatchedArguments())
        {
            std::wcout << L" unmatched> " << arg.data() << L"\n";
        }
    }

    // Finally,
    // if the implicit `--help` switch was triggered, or if the parsing failed, we should show the user any error
    // message and the usage information of our software.
    parser.PrintErrorAndHelpIfNeeded(res);

    // ... and we tell the application logic if cmd line parsing was successful.
    return res.IsSuccess();
}

void Config::PrintVersionInfo()
{
    std::wcout << L"::version: " << YACLAP_VERSION_MAJOR << L"." << YACLAP_VERSION_MINOR << L"." << YACLAP_VERSION_PATCH
               << L"." << YACLAP_VERSION_BUILD << L"\n";
}
