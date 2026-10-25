param([switch]$Check)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$ini = [IO.File]::ReadAllText((Join-Path $root 'deploy/RE3Controls.ini')).Replace("`r`n", "`n").TrimEnd("`n")
if ($ini.Contains(')RE5INI"')) { throw 'INI contains the C++ raw-string delimiter' }
$generated = 'R"RE5INI(' + $ini + "`n" + ')RE5INI"' + "`n"
$target = Join-Path $root 'src/default_ini.inc'
if ($Check) {
    if (!(Test-Path -LiteralPath $target) -or
        [IO.File]::ReadAllText($target).Replace("`r`n", "`n") -cne $generated) {
        throw 'Embedded configuration is stale; run tools/update-default-ini.ps1'
    }
} else {
    [IO.File]::WriteAllText($target, $generated, [Text.UTF8Encoding]::new($false))
}
