# yaclap
Yet Another Command Line Argument Parser, for c++, as ___single-header-only___ library, ___natively supporting `wchar_t`___, with a ___minimal feature set___, e.g. without advanced type conversion and validation, just because...

<!-- Release Remove Begin -->
[![Build and Test](https://github.com/sgrottel/yaclap/actions/workflows/build-and-test.yaml/badge.svg?branch=main)](https://github.com/sgrottel/yaclap/actions/workflows/build-and-test.yaml)
[![GitHub](https://img.shields.io/github/license/sgrottel/yaclap)](./LICENSE)
[![GitHub Release](https://img.shields.io/github/v/release/sgrottel/yaclap?logo=github)](https://github.com/sgrottel/yaclap/releases)
[![NuGet Version](https://img.shields.io/nuget/v/SGrottel.yaclap?logo=nuget)](https://www.nuget.org/packages/SGrottel.yaclap/)
<!-- Release Remove End -->
<!-- Release Uncomment Begin
%Version Info%

Additional information is available on the project webseit: [https://github.com/sgrottel/yaclap](https://github.com/sgrottel/yaclap)
Release Uncomment End -->


## Concepts
This command line parser is inspired by the [POSIX standard](https://en.wikipedia.org/wiki/POSIX) and especially the [System.CommandLine CSharp Library](https://learn.microsoft.com/en-us/dotnet/standard/commandline/).

**Features**

- `Command`s also called 'verbs', switching to specific sets of Options, Arguments, and Commands supported
- `Option`s are optionally named input, followed by a Argument value
- `Alias` names for Options, Switches, and Commands
- `Argument`s are values, either following an Option, or being identified by it's position in the stream of arguments, which are not otherwise matched.
  Arguments have names for the readability of the generated documentation only.
- `Switch`es are special Options without an explicit Argument
- `--` the double-dash token stops the parsing and will return all following arguments as _unmatched_
- Automatic `--help` documentation generation is supported
- Option-argument delimiters are supported: ` ` (space), `=`, and `:`
- Supports case-sensitive and case-insensitive matching of names of Options and Switches
- Supports basic value conversion of Option values and Argument values to basic types:
  long integer numbers, double-precision floating-point numbers, and boolean values

<!-- Release Remove Begin -->
**Not Supported**

- Input value validation, e.g. integer range check or string to keyword matching, is not supported by this library
- Complex value type conversion and validation is not supported by this library
  - Fetch arguments as strings and convert and validate the input with you own code.
  - There are utility functions to inject error messages into the parser output.
- Option prefixes, `-`, `--`, and `/`, are part of the Option's name, and have no further special meaning
- Automatic aliasing, e.g. shortening option names, is not supported
- Option bunding (e.g. `-f -d -x` = `-fdx`) is not supported

<!-- Release Remove End -->

## Usage Example
<!-- Release Uncomment Begin
You define the command line argument syntax for your application via objects of the types `Command`, `Option`, `Switch`, and `Argument`, added to either `Command` objects or your main `Parser` object.
```cpp
Option inputOption{
    {_T("--input"), StringCompare::CaseInsensitive},
    _T("file"),
    _T("An input file")};
Switch verboseSwitch{
    {_T("--verbose"), StringCompare::CaseInsensitive},
    _T("Verbosity switch")};
Command commandA{
    {_T("CommandA"), StringCompare::CaseInsensitive},
    _T("Command A")};
commandA
    .Add(inputOption);
parser
    .Add(commandA)
    .Add(verboseSwitch);
```
The parser result can then be easily assigned your variables:
```cpp
Parser::Result res = parser.Parse(argc, argv);

auto andValue = res.GetArgument(andArgument);
if (andValue)
{
    m_andArg = andValue;
}

m_boolValue = res.GetOptionValue(boolValueOption, Parser::Result::ErrorIfMultiple).AsBool().value_or(false);
```

See the [online documentation](https://github.com/sgrottel/yaclap/?tab=readme-ov-file#usage-examples) and [online example file `src/cmdargs.cpp`](https://github.com/sgrottel/yaclap/blob/main/src/cmdargs.cpp) for more information.
Release Uncomment End -->
<!-- Release Remove Begin -->
For a extensive usage example, see [src/cmdargs.cpp](./src/cmdargs.cpp).

It's recommended to separate the command line arguments parsing code from the rest of your application logic, as seen in this example.
The `struct Config` defined in [src/cmdargs.h](./src/cmdargs.hpp#L18) publishes the parsed configuration to your application.
All implementation details and dependencies to `yaclap` are isolated in [src/cmdargs.cpp](./src/cmdargs.cpp).

### 1. the `Parser` Object
Instantiate a [`Parser` object](./src/cmdargs.cpp#L24) with your application executable name and a description.
This information will be used for the automatically generated usage help.
The `Parser` object will be your entry point for defining the syntax of the command line arguments and for performing the actual parsing operation.

### 2. `Option`s and `Switch`es
`Option`s and `Switch`es are named structures of your command line argument syntax.
`Option`s are followed by one value argument.
`Switch`es have no explicit argument.
As with most named structures, `Option`s and `Switch`es can have additional alias names, e.g. a shorter version if they are used very often.

Examples:
```cpp
Option inputOption{
    {_T("--input"), StringCompare::CaseInsensitive},
    _T("file"),
    _T("An input file")};
inputOption
    .AddAlias(_T("-i"))
    .AddAlias(_T("/i"));
```

Call as:
* `your.exe --input c:\path\to\file.txt`
* `your.exe -i c:\path\to\file.txt`
* `your.exe /i c:\path\to\file.txt`

```cpp
Switch verboseSwitch{
    {_T("--verbose"), StringCompare::CaseInsensitive},
    _T("Verbosity switch")};
verboseSwitch
    .AddAlias(_T("-v"))
    .AddAlias(_T("/v"));
```

Call as:
* `your.exe --verbose`
* `your.exe -v`
* `your.exe /v`

### 3. `Argument`s
`Argument`s are named command line arguments.
They are purely defined by their order or appareance when specified.

Example:
```cpp
Argument andArgument{
    _T("string"),
    _T("An additional string argument")};
```

`Option`s and `Switch`es, which are prefixed by their names, have priority over matching an argument.

If there are more command line arguments specified when calling your application than the syntax structure defines, those are so called `unnamed arguments`.
By default, [`unnamed arguments` will set an error](./src/cmdargs.cpp#L29).

### 4. `Command`s
`Command`s can structure your command line argument syntax into groups.
Think of it as mode-switches for your application.

Matching `Command` names during parsing takes priority over matching `Option`s, `Switch`es, and `Argument`s.

When specifying your command line argument syntax, you can add `Option`s, `Switch`es, `Argument`s, and `Command`s either to the `Parser` object, or to `Command` objects.
The parser will start matching the command line against the objects added to the `Parser` object.

As soon as an argument matches a name of a `Command` object, the parser "switches modes" to this command:
* It will now match `Command`s added to this `Command` object, and no longer `Command`s added to the `Parser` object.
* It will now match `Option`s, `Switch`es, and `Argument`s which are either added to the `Parser` object or the `Command` object.

Example:
```cpp
Command commandA{
    {_T("CommandA"), StringCompare::CaseInsensitive},
    _T("Command A")};
commandA
    .AddAlias({_T("CmdA"), StringCompare::CaseInsensitive})
    .AddAlias(_T("A"))
    .Add(inputOption);
```
```cpp
parser
    .Add(commandA)
    .Add(verboseSwitch);
```

Call as:
* `your.exe -v` -- works, as the `verboseSwitch` is added to the `Parser`.
* `your.exe CmdA -i c:\path\file.txt` -- The `inputOption` is *only* available after the `Parser` switched to the "mode" of `Command commandA`.
* `your.exe CmdA -v` -- works, because the verbose switch remains available event after switching to the "mode" of `Command commandA`.

### 5. Values
The `Parser::Result` only structures the input command line, e.g. selects the input strings based on the defined syntax:
```cpp
Parser::Result res = parser.Parse(argc, argv);
```

Elements without values, i.e. `Command`s and `Switch`es, are queried if they were present in the command line or how often they were present in the command line:
```cpp
if (res.HasCommand(commandA))
```
```cpp
m_verbose = static_cast<int>(res.HasSwitch(verboseSwitch));
```

Elements with values, i.e. `Option`s and `Argument`s, are queried either by receiving the first occurance, optionally setting and error if the `Option` occured multiple times in the command line, or by receiving all occurances:
```cpp
auto inputValue = res.GetOptionValue(inputOption, Parser::Result::ErrorIfMultiple);
```
```cpp
auto andValue = res.GetArgument(andArgument);
```
```cpp
for (Parser::ResultValueView const& s : res.GetOptionValues(intValueOption))
```

The returned `Parser::ResultValueView` objects are `string_view`s to the original input strings.
These objects can also be queried via `IsFromSource` if they were created from matching a specific element, e.g. `Option` or named `Argument`, and at which position in the original input argument list they were.

Additionally, the `Parser::ResultValueView` offers three utility functions to convert the value from a raw string to other data types:
* `std::optional<long long> Parser::ResultValueView::AsInteger(...)`
* `std::optional<double> Parser::ResultValueView::AsDouble(...)`
* `std::optional<bool> Parser::ResultValueView::AsBool(...)`

See the [code documentation of these functions](./include/yaclap.hpp#L771) for details on the accepted input syntax.

These utility functions allow for easy assignments of such values to the configruation struct:
```cpp
auto dValOpt = res.GetOptionValue(doubleValueOption, Parser::Result::ErrorIfMultiple).AsDouble();
if (dValOpt)
{
    m_doubleValue = dValOpt.value();
}
```
or
```cpp
m_boolValue = res.GetOptionValue(boolValueOption, Parser::Result::ErrorIfMultiple).AsBool().value_or(false);
```

### 6. Error Handling
The `Parser::Result` object also holds a general success flag and an error message if one was set.

It is recommended to [show the error message and possibly usage help information automatically](./src/cmdargs.cpp#L208):
```cpp
parser.PrintErrorAndHelpIfNeeded(res);
```

And, you should [inform the your application's main logic whether or not the command line argument parsing function was successful](./src/cmdargs.cpp#L211):
```cpp
return res.IsSuccess() && !res.ShouldShowHelp();
```
Note: this example code returns a none-success value if the internal automatic help switch was invoked by the user.
The help text was already printed by `PrintErrorAndHelpIfNeeded` above.
So, returning `false` here avoids further program execution.
You can, of course, implement other means to stop the program after showing the help text, to distinguish this operation from error cases.


## Integration into your Application
You can either use the Nuget package (recommended) or directly add the source code.

### Integration via Nuget (Recommended)
Add the Nuget package [`SGrottel.yaclap`](https://www.nuget.org/packages/SGrottel.yaclap/) to your project.
The include path will automatically be set accordingly, so you can
```cpp
#include "yaclap.hpp"
```

### Integration via Code Copy
Alternatively you can just copy the contents of a [release zip](https://github.com/sgrottel/yaclap/releases) or the contents of the [include](./include) directory to your project (not recommended).
It's recommended you place the files into a separate subdirectory and add that directory to your include directories compiler options, for separation of concerns.


## Report Issues
If you found an issue, please have a look at the [Issues list](https://github.com/sgrottel/yaclap/issues).

If it is an already existing issue, feel free to up-vote it.
If you can contribute additional information, e.g. further reproduction steps, please comment on the issue.

If you do not find a matching issue, please create one, with all steps necessary to reproduce the issue, and all further information you can provide.

If you already have a fix ready, please, create and issue and a linked [Pull request](https://github.com/sgrottel/yaclap/pulls).

Thank you!


## Contributing
Contributions are very welcome!

Please, create [Issues](https://github.com/sgrottel/yaclap/issues) and [Pull requests](https://github.com/sgrottel/yaclap/pulls).

Please, keep in mind, that one of the major goals of the project is to create a _minimalistic_ library.
Many sophisticated functionality might get rejected, if it would wide the scope of the library too much.

In doubt, please reach out to me, via message, e-mail or [Issue](https://github.com/sgrottel/yaclap/issues).

Thank you!


## Alternatives
There are plenty of alternatives.
Especially, if your project is already using a larger framework, you likely already have something available to you.

* [Qt `QCommandLineParser` (https://doc.qt.io/qt-6/qcommandlineparser.html)](https://doc.qt.io/qt-6/qcommandlineparser.html)
* [`Boost.Program_options` (https://www.boost.org/doc/libs/1_57_0/doc/html/program_options.html)](https://www.boost.org/doc/libs/1_57_0/doc/html/program_options.html)
* [GNU `getopt`](https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html)
* [Google `gflags`](https://github.com/gflags/gflags)
* ...
* [Finding alternative cpp command line argument parsers](https://letmegooglethat.com/?q=cpp+command+line+arguments+parser)

The "minimalistic" and the "native `wchar_t` support" features are the reasons I started this library.
<!-- Release Remove End -->


## License
This library is freely available as open source under the [MIT License](./LICENSE):

> MIT License
> 
> Copyright (c) 2024-2025 Sebastian Grottel
> 
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
> 
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
> 
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.

If you are using this library in an open-source software, the above copyright notice must not be removed from the `yaclap.hpp` file.
But you do not need to reproduce the copyright notice in the compiled binary or accompanying documentation.
