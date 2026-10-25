param([string]$Name = 'RE5-Control-Plus-1.9.4-beta.1')
$ErrorActionPreference = 'Stop'
if ($Name -notmatch '^[A-Za-z0-9._-]+$') { throw 'Package name must be a simple file name' }
$destination = Join-Path $PSScriptRoot "build/$Name"
if (Test-Path -LiteralPath $destination) { throw "Package directory exists; choose a fresh name: $destination" }
New-Item -ItemType Directory $destination | Out-Null
Copy-Item -LiteralPath "$PSScriptRoot/build/xinput1_3.dll" -Destination $destination
Copy-Item -LiteralPath "$PSScriptRoot/deploy/RE3Controls.ini" -Destination $destination
Copy-Item -LiteralPath "$PSScriptRoot/docs/README_RE5Plus_ZH.md" -Destination $destination
Copy-Item -LiteralPath "$PSScriptRoot/docs/README_EN.md" -Destination $destination
Copy-Item -LiteralPath "$PSScriptRoot/CHANGELOG.md" -Destination $destination
Copy-Item -LiteralPath "$PSScriptRoot/THIRD_PARTY_NOTICES.md" -Destination $destination
Copy-Item -LiteralPath "$PSScriptRoot/LICENSE-TrainerReference.txt" -Destination $destination
Copy-Item -LiteralPath "$PSScriptRoot/docs/RE3Controls_完整中文说明.md" -Destination $destination
Copy-Item -LiteralPath "$PSScriptRoot/LICENSE" -Destination "$destination/LICENSE-RE3Controls.txt"
Copy-Item -LiteralPath "$PSScriptRoot/LICENSE-RE5Fix.txt" -Destination $destination
Copy-Item -LiteralPath "$PSScriptRoot/third_party/minhook/LICENSE.txt" -Destination "$destination/LICENSE-MinHook.txt"
Copy-Item -LiteralPath "$PSScriptRoot/docs/user_README_EN.txt" -Destination "$destination/README-RE3Controls-upstream.txt"
$hash = (Get-FileHash -LiteralPath "$destination/xinput1_3.dll" -Algorithm SHA256).Hash
$checksums = Get-ChildItem -LiteralPath $destination -File | Sort-Object Name | ForEach-Object {
    "$( (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash )  $($_.Name)"
}
Set-Content -LiteralPath "$destination/SHA256SUMS.txt" -Value $checksums -Encoding utf8
$archive = Join-Path $PSScriptRoot "../$Name.zip"
Compress-Archive -LiteralPath $destination -DestinationPath $archive -Force
Get-Item -LiteralPath $archive | Select-Object FullName,Length
Get-FileHash -LiteralPath "$destination/xinput1_3.dll" -Algorithm SHA256
