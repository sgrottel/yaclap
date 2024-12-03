#
# yaclap - package-release.ps1
#
# Script to create and package all files in the current checkout for release as zip and nuget
#
param(
	[string]$buildNumber = $null
)
cd $PSScriptRoot
$ErrorActionPreference = "Stop"


# Get git hash as version complement
$githash = (git rev-parse HEAD)
Write-Host "Githash: $githash"


# Read version from `yaclap.hpp`
# and inject build number and git hash into `yaclap.hpp`
$yaclap = get-content ..\include\yaclap.hpp | Out-String

$matches = [Text.RegularExpressions.MatchCollection]([Regex]::Matches($yaclap, '(?mi)^\s*#define\s+YACLAP_VERSION_MAJOR\s+(\d+)\s*$'))
if ($matches.count -ne 1) {
	Write-Error "CSharp: failed to identify exactly one YACLAP_VERSION_MAJOR field"
}
$verMajor = [int]$matches[0].Groups[1].value

$matches = [Text.RegularExpressions.MatchCollection]([Regex]::Matches($yaclap, '(?mi)^\s*#define\s+YACLAP_VERSION_MINOR\s+(\d+)\s*$'))
if ($matches.count -ne 1) {
	Write-Error "CSharp: failed to identify exactly one YACLAP_VERSION_MINOR field"
}
$verMinor = [int]$matches[0].Groups[1].value

$matches = [Text.RegularExpressions.MatchCollection]([Regex]::Matches($yaclap, '(?mi)^\s*#define\s+YACLAP_VERSION_PATCH\s+(\d+)\s*$'))
if ($matches.count -ne 1) {
	Write-Error "CSharp: failed to identify exactly one YACLAP_VERSION_PATCH field"
}
$verPatch = [int]$matches[0].Groups[1].value

$matches = [Text.RegularExpressions.MatchCollection]([Regex]::Matches($yaclap, '(?mi)^\s*#define\s+YACLAP_VERSION_BUILD\s+(\d+)\s*$'))
if ($matches.count -ne 1) {
	Write-Error "CSharp: failed to identify exactly one YACLAP_VERSION_BUILD field"
}
$verBuild = [int]$matches[0].Groups[1].value

Write-Host "Read version from 'yaclap.h': $verMajor.$verMinor.$verPatch.$verBuild"

if ($buildNumber) {
	$verBuild = $buildNumber
	$yaclap = [Regex]::Replace($yaclap, '(?mi)^\s*#define\s+YACLAP_VERSION_BUILD\s+(\d+)\s*$', "#define YACLAP_VERSION_BUILD $verBuild")
}

$verStr = "$verMajor.$verMinor.$verPatch"
if ($verBuild -and $verBuild -ne 0) {
	$verStr += ".$verBuild"
}

$yaclap = [Regex]::Replace($yaclap, '(?mi)^\s*#define\s+YACLAP_VERSION_GITHASHSTR.*$', "#define YACLAP_VERSION_GITHASHSTR `"$githash`"")
set-content -path ..\include\yaclap.hpp -value ($yaclap -replace "`r`n","`n") -nonewline


# Build `ComponentSource.json`
$componentSource = @{
	_type = "ComponentSourceManifest"
	_version = 1
	components = @(
		@{
			name = "SGrottel yaclap"
			source = @(
				@{
					type = "git"
					url =  "https://www.github.com/sgrottel/yaclap"
					version = $verStr
					hash = $githash
				}
			)
		}
	)
}
set-content -path ..\include\ComponentSource.json -value ($componentSource | ConvertTo-Json -depth 100)


# Build release version of `README.md`
$readme = [string[]](get-content ..\README.md)
$rm = @()
$addLine = $true
foreach ($line in $readme) {
	if ($line -match "^\s*<!--\s*Release\s+Remove\s+Begin\s*-->.*") {
		$addLine = $false
	}
	if ($line -match "^\s*<!--\s*Release\s+Uncomment\s+Begin.*") {
		continue;
	}
	if ($line -match "^\s*Release\s+Uncomment\s+End\s*-->.*") {
		continue;
	}
	if ($line -match "^%Version\s+Info%.*") {
		$line = "Version: $verStr`r`nGit Hash: $githash"
	}

	if ($addLine) {
		$rm += $line
	}

	if ($line -match "^\s*<!--\s*Release\s+Remove\s+end\s*-->.*") {
		$addLine = $true
	}
}
set-content -path ..\include\README.md -value $rm


# Package release zip
Write-Host "Release zip: SGrottel.yaclap.$verStr.zip"
Compress-Archive -path ..\include\*.*,..\LICENSE -DestinationPath "SGrottel.yaclap.$verStr.zip" -force


# Package nuget
Write-Host "Release nuget:"
nuget.exe pack SGrottel.yaclap.nuspec -version $verStr


Write-Host "done."
