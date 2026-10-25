param([Parameter(Mandatory)][string]$PackageDirectory)
$ErrorActionPreference = 'Stop'
$required = @('xinput1_3.dll', 'RE3Controls.ini', 'README_EN.md',
    'README_RE5Plus_ZH.md', 'RE3Controls_完整中文说明.md', 'CHANGELOG.md',
    'THIRD_PARTY_NOTICES.md', 'LICENSE-RE3Controls.txt', 'LICENSE-RE5Fix.txt',
    'LICENSE-MinHook.txt', 'LICENSE-TrainerReference.txt',
    'README-RE3Controls-upstream.txt', 'SHA256SUMS.txt')
$files = @(Get-ChildItem -LiteralPath $PackageDirectory -File)
if (@(Compare-Object ($required | Sort-Object) ($files.Name | Sort-Object)).Count) {
    throw 'Package manifest mismatch'
}
$ini = Get-Content -LiteralPath (Join-Path $PackageDirectory 'RE3Controls.ini') -Raw
foreach ($key in 'InfiniteHealth','PartnerInfiniteHealth','InfiniteAmmo','NoReload','InfiniteGrenades') {
    if ($ini -notmatch "(?m)^$key=0\r?$") { throw "Trainer default must be off: $key" }
}
if ($ini -match 'TurretNoOverheat') { throw 'Removed heat option remains in release INI' }
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
foreach ($line in Get-Content -LiteralPath (Join-Path $PackageDirectory 'SHA256SUMS.txt')) {
    if ($line -notmatch '^([A-F0-9]{64})  (.+)$') { throw 'Invalid checksum line' }
    $actual = (Get-FileHash -LiteralPath (Join-Path $PackageDirectory $Matches[2])).Hash
    if ($actual -ne $Matches[1]) { throw 'Checksum mismatch' }
}
$details = (Get-Item -LiteralPath $dll).VersionInfo
if ($details.ProductName -ne 'RE5 Control Plus (RE3Controls fork)' -or
    $details.ProductVersion -ne '1.9.4-beta.1') { throw 'Unexpected product metadata' }
Write-Output 'PASS: release manifest, default-off helpers, removed heat option, x86 DLL, checksums and product version.'
