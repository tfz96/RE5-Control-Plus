param([string]$Name = 'RE5-Control-Plus-1.9.4')
$ErrorActionPreference = 'Stop'
if ($Name -notmatch '^[A-Za-z0-9._-]+$') { throw 'Package name must be a simple file name' }
$destination = Join-Path $PSScriptRoot "build/$Name"
if (Test-Path -LiteralPath $destination) { throw "Package directory exists; choose a fresh name: $destination" }
New-Item -ItemType Directory $destination | Out-Null
Copy-Item -LiteralPath "$PSScriptRoot/build/xinput1_3.dll" -Destination $destination
Copy-Item -LiteralPath "$PSScriptRoot/deploy/RE3Controls.ini" -Destination $destination
$readme = [IO.File]::ReadAllText("$PSScriptRoot/README.md")
$readme += "`n`n---`n`n# Included Notices And Licenses / 开源署名与许可原文`n`n"
$readme += [IO.File]::ReadAllText("$PSScriptRoot/THIRD_PARTY_NOTICES.md")
$licenses = [ordered]@{
    'RE3Controls - MIT' = 'LICENSE'
    'RE5Fix - MIT' = 'LICENSE-RE5Fix.txt'
    'MinHook - BSD 2-Clause' = 'third_party/minhook/LICENSE.txt'
    'Trainer Reference - MIT' = 'LICENSE-TrainerReference.txt'
}
foreach ($entry in $licenses.GetEnumerator()) {
    $readme += "`n`n## $($entry.Key)`n`n"
    $readme += [IO.File]::ReadAllText((Join-Path $PSScriptRoot $entry.Value))
}
[IO.File]::WriteAllText("$destination/README.md", $readme, [Text.UTF8Encoding]::new($false))
$archive = Join-Path $PSScriptRoot "../$Name.zip"
Compress-Archive -Path "$destination/*" -DestinationPath $archive -Force
Get-Item -LiteralPath $archive | Select-Object FullName,Length
Get-FileHash -LiteralPath $archive -Algorithm SHA256
