# yaclap
Yet Another Command Line Argument Parser, for c++, as ___single-header-only___ library, ___natively supporting `wchar_t`___, with a ___minimal feature set___, e.g. without type conversion and validation, just because...

[![Build and Test](https://github.com/sgrottel/yaclap/actions/workflows/build-and-test.yaml/badge.svg?branch=main)](https://github.com/sgrottel/yaclap/actions/workflows/build-and-test.yaml)
[![GitHub](https://img.shields.io/github/license/sgrottel/yaclap)](./LICENSE)


## Concepts
This command line parser is inspired by the [POSIX standard](https://en.wikipedia.org/wiki/POSIX) and especially the [System.CommandLine CSharp Library](https://learn.microsoft.com/en-us/dotnet/standard/commandline/).

**Features**

- `Commands` also called 'verbs', switching to specific sets of Options, Arguments, and Commands supported
- `Options` are optionally named input, followed by a Argument value
- `Alias` names for Options, Switches, and Commands
- `Arguments` are values, either following an Option, or being identified by it's position in the stream of arguments, which are not otherwise matched.
  Arguments have names for the readability of the generated documentation only.
- `Switches` are special Options without an explicit Argument
- `--` the double-dash token stops the parsing and will return all following arguments as _unmatched_
- Automatic `--help` documentation generation is supported
- Option-argument delimiters are supported: ` ` (space), `=`, and `:`
- Supports case-sensitive and case-insensitive matching of names of Options and Switches
- Supports basic value conversion of Option values and Argument values to basic types:
  long integer numbers, double-precision floating-point numbers, and boolean values

**Not Supported**

- Input value validation, e.g. integer range check or string to keyword matching, is not supported by this library
- Complex value type conversion and validation is not supported by this library
  - Fetch arguments as strings and convert and validate the input with you own code.
  - There are utility functions to inject error messages into the parser output.
- Option prefixes, `-`, `--`, and `/`, are part of the Option's name, and have no further special meaning
- Automatic aliasing, e.g. shortening option names, is not supported
- Option bunding (e.g. `-f -d -x` = `-fdx`) is not supported


## Integration and Examples

TODO


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

[Finding alternative cpp command line argument parsers](https://letmegooglethat.com/?q=cpp+command+line+arguments+parser)

The "minimalistic" and the "native `wchar_t` support" features are the reasons I started this library.


## License
This library is freely available as open source under the [MIT License](./LICENSE):

> MIT License
> 
> Copyright (c) 2024 Sebastian Grottel
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
