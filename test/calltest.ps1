#
# yaclap - calltest.ps1
#
# Test script calling yacalp.exe with different command line options, checking the expected result
#
param
(
	[Parameter(Mandatory)][IO.FileInfo]$exe
)
$ErrorActionPreference = [Management.Automation.ActionPreference]::Continue

if (-not $exe.Exists)
{
	Write-Error "Specified test executable does not exist {$exe}"
	Exit 1
}

$tests =
@(
	@{
		"args"= @("cmda","-i","whateff.txt","-v","-v");
		"regs" = @("^::::oA2i0_0_f__$", "^whateff\.txt$");
	},
	@{
		"args" = @("B","/V","42","-v","-v","-v","and");
		"regs" = @("^::::oB3_42_0_f_a$", "^and$");
	},
	@{
		"args" = @("--help");
		"regs" = @("^Usage:$", "^\s+yaclap\.exe\s+\[command\]\s+\[options\]$");
	},
	@{
		"args" = @("B","/V","7","and","or");
		"regs" = @("^::::oB0_7_0_f_a$", "^and | or$");
	},
	@{
		"args" = @("B", "and", "--", "--value", "5");
		"regs" = @("^::::oB0_0_0_f_a$", "^Unmatched arguments: 2$", "^\s*unmatched> --value$", "^\s*unmatched> 5$", "^and$");
	},
	@{
		"args" = @("B", "and", "--value", "-b110 ", "--double", "314.159265e-2", "--bool", " true ");
		"regs" = @("^::::oB0_-6_3\.141[0-9]*_t_a$");
	}
);

Write-Host "Clear call:"
& $exe

Write-Host
Write-Host $tests.count "tests"
$countTotal = 0;
$countOk = 0;
$countFail = 0;
foreach ($test in $tests)
{
	Write-Host "-" $exe.name $test.args
	$output = (& $exe $test.args) | Out-String
	# Write-Host "exit code:" $LastExitCode
	$hasFail = $false;
	foreach ($reg in $test.regs)
	{
		$countTotal++
		if ($reg.EndsWith("$") -and -not $reg.EndsWith("\r?$"))
		{
			$reg = $reg.SubString(0, $reg.length - 1) + "\r?$";
		}
		$succ = $output -match "(?m)$reg"
		if ($succ)
		{
			Write-Host "    $reg ✅"
			$countOk++
		}
		else
		{
			$ErrorView = 'ConciseView'
			Write-Host "    $reg ❌"
			$hasFail = $true
			$countFail++
		}
	}

	if ($hasFail) {
		Write-Host "`n>> Begin Output <<"
		Write-Host $output
		Write-Host ">> End Output <<`n"
	}
}

Write-Host "Tests:`n  Total: $countTotal`n  Ok   : $countOk`n  Fail : $countFail"
if ($countFail -gt 0)
{
	exit 1
}
