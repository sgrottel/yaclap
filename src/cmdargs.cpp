
#include "yaclap.hpp"

#include "cmdargs.h"

bool Config::ParseCmdLine(int argc, const _TCHAR* const* argv)
{
	yaclap::Parser<_TCHAR> parser;

	yaclap::Option<_TCHAR> inputOption(_T("--input"), _T("An input file"));
	inputOption.AddAlias(_T("-i")).AddAlias(_T("/i"));

	// TODO: Implement

	return false;
}
