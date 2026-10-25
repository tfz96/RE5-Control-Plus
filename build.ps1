param(
    [string]$Zig = 'zig',
    [ValidateSet('release', 'dev', 'test')][string]$Mode = 'release'
)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    & "$PSScriptRoot/tools/update-default-ini.ps1" -Check
    New-Item -ItemType Directory -Force build,build/dev | Out-Null
    $mh = 'third_party/minhook'
    foreach ($unit in @('hook', 'buffer', 'trampoline', 'hde/hde32')) {
        $name = ($unit -split '/')[-1]
        & $Zig cc -target x86-windows-gnu -O2 -c "$mh/src/$unit.c" -o "build/mh_$name.o" "-I$mh/include"
        if ($LASTEXITCODE) { throw "MinHook compile failed: $unit" }
    }
    $flags = @('-target', 'x86-windows-gnu', '-O2', '-std=c++17', '-fno-exceptions',
               '-fno-rtti', "-I$mh/include", '-Isrc')
    $objects = @(Get-ChildItem build/mh_*.o | ForEach-Object FullName)
    if ($Mode -eq 'test') {
        foreach ($test in @('enhancements', 'wheel', 'trainer', 'config')) {
            $testSources = @("tests/${test}_test.cpp")
            if ($test -eq 'config') { $testSources += 'src/config.cpp' }
            & $Zig c++ @flags -UNDEBUG @testSources @objects -o "build/${test}_test.exe" -luser32 -lkernel32
            if ($LASTEXITCODE) { throw "Test compilation failed: $test" }
            & "./build/${test}_test.exe"
            if ($LASTEXITCODE) { throw "Tests failed: $test" }
        }
        return
    }
    $sources = @(Get-ChildItem src/*.cpp | Where-Object {
        $Mode -eq 'dev' -or $_.Name -notin @('devserver.cpp', 'd3d9_hook.cpp')
    } | ForEach-Object FullName)
    $output = 'build/xinput1_3.dll'
    $links = @('-luser32', '-lkernel32')
    if ($Mode -eq 'dev') {
        $flags += '-DRE3C_DEV=1'
        $links += '-lws2_32'
        $output = 'build/dev/xinput1_3.dll'
    } else {
        $flags += @('-g0', '-s')
    }
    & $Zig c++ @flags -shared @sources @objects src/xinput1_3.def src/version.rc -o $output @links
    if ($LASTEXITCODE) { throw 'DLL compilation failed' }
    Get-FileHash $output -Algorithm SHA256
} finally {
    Pop-Location
}
