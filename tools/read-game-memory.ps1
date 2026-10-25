param(
    [Parameter(Mandatory)][uint32]$Address,
    [Parameter(Mandatory)][int]$Length,
    [Parameter(Mandatory)][string]$OutputPath
)
$ErrorActionPreference = 'Stop'
Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class GameMemoryReader {
    [DllImport("kernel32.dll", SetLastError=true)]
    public static extern IntPtr OpenProcess(uint access, bool inherit, int id);
    [DllImport("kernel32.dll", SetLastError=true)]
    public static extern bool ReadProcessMemory(IntPtr process, IntPtr address,
        byte[] buffer, UIntPtr size, out UIntPtr read);
    [DllImport("kernel32.dll")]
    public static extern bool CloseHandle(IntPtr handle);
}
'@
$game = Get-Process re5dx9 | Select-Object -First 1
$handle = [GameMemoryReader]::OpenProcess(0x10, $false, $game.Id)
if ($handle -eq [IntPtr]::Zero) { throw 'Cannot open game for reading' }
try {
    $bytes = New-Object byte[] $Length
    $count = [UIntPtr]::Zero
    if (![GameMemoryReader]::ReadProcessMemory($handle, [IntPtr][long]$Address, $bytes,
            [UIntPtr]$Length, [ref]$count) -or $count.ToUInt64() -ne $Length) {
        throw "Cannot read requested memory: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"
    }
    [IO.File]::WriteAllBytes($ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutputPath), $bytes)
} finally {
    [void][GameMemoryReader]::CloseHandle($handle)
}
