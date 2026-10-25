param([Parameter(Mandatory)][string]$PackageDirectory)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
& "$root/tools/update-default-ini.ps1" -Check
$required = @('xinput1_3.dll', 'RE3Controls.ini', 'README.md')
$files = @(Get-ChildItem -LiteralPath $PackageDirectory -Recurse -File)
if (@(Compare-Object ($required | Sort-Object) ($files.Name | Sort-Object)).Count) {
    throw 'Package must contain exactly DLL, INI and README'
}
$ini = [IO.File]::ReadAllText((Join-Path $PackageDirectory 'RE3Controls.ini'))
foreach ($key in 'InfiniteHealth','PartnerInfiniteHealth','InfiniteAmmo','NoReload','InfiniteGrenades') {
    if ($ini -notmatch "(?m)^$key=0\r?$") { throw "Trainer default must be off: $key" }
}
if ($ini -match 'TurretNoOverheat') { throw 'Removed heat option remains in release INI' }
$comments = @()
$keyCount = 0
foreach ($line in ($ini -split '\r?\n')) {
    if ($line -match '^;') { $comments += $line; continue }
    if ($line -match '^\[') { $comments = @(); continue }
    if ($line -match '^([A-Za-z]\w*)=') {
        $key = $Matches[1]
        if (($comments -join "`n") -notmatch '[\u4e00-\u9fff]' -or
            ($comments -join "`n") -notmatch '[A-Za-z]{3}') {
            throw "Missing bilingual comments: $key"
        }
        $keyCount++
        $comments = @()
    }
}
if ($keyCount -lt 60) { throw 'Configuration settings unexpectedly missing' }
foreach ($name in 'RE3Controls.ini', 'xinput1_3.dll') {
    $original = if ($name -eq 'RE3Controls.ini') { "$root/deploy/$name" } else { "$root/build/$name" }
    if ((Get-FileHash -LiteralPath $original).Hash -ne
        (Get-FileHash -LiteralPath (Join-Path $PackageDirectory $name)).Hash) {
        throw "Packaged file differs from release source: $name"
    }
}
$readme = [IO.File]::ReadAllText((Join-Path $PackageDirectory 'README.md')).Replace("`r`n", "`n")
foreach ($relative in 'README.md','THIRD_PARTY_NOTICES.md','LICENSE','LICENSE-RE5Fix.txt',
    'LICENSE-TrainerReference.txt','third_party/minhook/LICENSE.txt') {
    $content = [IO.File]::ReadAllText((Join-Path $root $relative)).Replace("`r`n", "`n").Trim()
    if (!$readme.Contains($content)) { throw "Missing complete documentation/license: $relative" }
}
if (!$readme.Contains('OpenAI Codex') -or !$readme.Contains('tfz96')) { throw 'Missing fork credits' }
$dll = Join-Path $PackageDirectory 'xinput1_3.dll'
$stream = [IO.File]::OpenRead($dll)
try {
    $reader = [IO.BinaryReader]::new($stream)
    if ($reader.ReadUInt16() -ne 0x5A4D) { throw 'Missing DOS header' }
    $stream.Position = 0x3C
    $offset = $reader.ReadUInt32()
    $stream.Position = $offset
    if ($reader.ReadUInt32() -ne 0x4550 -or $reader.ReadUInt16() -ne 0x14C) {
        throw 'Release DLL must be a Windows x86 PE'
    }
} finally { $stream.Dispose() }
$details = (Get-Item -LiteralPath $dll).VersionInfo
if ($details.ProductName -ne 'RE5 Control Plus (RE3Controls fork)' -or
    $details.ProductVersion -ne '1.9.4') { throw 'Unexpected product metadata' }
$archive = Join-Path $root "../$(Split-Path $PackageDirectory -Leaf).zip"
$zip = [IO.Compression.ZipFile]::OpenRead($archive)
try {
    if (@(Compare-Object ($required | Sort-Object) ($zip.Entries.FullName | Sort-Object)).Count) {
        throw 'ZIP must contain exactly three files at its root'
    }
    foreach ($entry in $zip.Entries) {
        $entryStream = $entry.Open()
        try {
            if ((Get-FileHash -InputStream $entryStream).Hash -ne
                (Get-FileHash -LiteralPath (Join-Path $PackageDirectory $entry.FullName)).Hash) {
                throw "ZIP content mismatch: $($entry.FullName)"
            }
        } finally { $entryStream.Dispose() }
    }
} finally { $zip.Dispose() }
Write-Output "PASS: $keyCount bilingual settings, synced defaults, three-file ZIP, complete licenses/credits, x86 DLL and version."
