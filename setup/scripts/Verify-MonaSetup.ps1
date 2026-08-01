<#
.SYNOPSIS
  Sanity-check mona3 files and common lab paths (does not launch WinDBG).
#>
[CmdletBinding()]
param(
    [string]$InstallDir = "C:\Tools\mona3",
    [string]$RepoRoot = ""
)

$ErrorActionPreference = "Continue"
$failed = 0

function Ok($msg) { Write-Host "[OK] $msg" -ForegroundColor Green }
function Bad($msg) { Write-Host "[!!] $msg" -ForegroundColor Red; $script:failed++ }
function Info($msg) { Write-Host "[..] $msg" }

if (-not $RepoRoot) {
    $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
}

# mona install
foreach ($f in @("mona.py", "windbglib.py")) {
    $p = Join-Path $InstallDir $f
    if (Test-Path $p) {
        $line = Get-Content $p -TotalCount 1
        if ($line -match "<!DOCTYPE|<html") { Bad "$p looks like HTML" }
        else { Ok "$p exists and looks like text/code" }
    }
    else {
        Bad "Missing $p — run Install-Mona3.ps1"
    }
}

# mirror
$mirror = Join-Path $RepoRoot "tools\mona3\mona.py"
if (Test-Path $mirror) { Ok "Repo mirror present: $mirror" }
else { Info "No tools/mona3 mirror (optional if InstallDir is populated)" }

# py launcher
if (Get-Command py -ErrorAction SilentlyContinue) {
    Ok "Python launcher 'py' is on PATH"
    try {
        py -0p 2>$null | ForEach-Object { Info "  $_" }
    } catch {}
}
else {
    Info "Python launcher 'py' not found — CorelanPyKDInstall.ps1 may still have installed Python"
}

# WinDbg
$wdPaths = @(
    "${env:LOCALAPPDATA}\Microsoft\WindowsApps\WinDbgX.exe",
    "${env:ProgramFiles}\WindowsApps",
    "${env:ProgramFiles(x86)}\Windows Kits\10\Debuggers\x64\windbg.exe",
    "${env:ProgramFiles(x86)}\Windows Kits\10\Debuggers\x86\windbg.exe"
)
$foundDbg = $false
foreach ($p in $wdPaths) {
    if (Test-Path $p) { Ok "Debugger path exists: $p"; $foundDbg = $true }
}
if (-not $foundDbg) {
    Info "WinDbg not found in common paths — install via winget or SDK"
}

# ProcessMitigations module
try {
    Import-Module ProcessMitigations -ErrorAction Stop
    Ok "ProcessMitigations module available"
}
catch {
    Info "ProcessMitigations module not available (optional; used by Disable-TargetMitigations.ps1)"
}

# lab sources
foreach ($rel in @(
    "labs\01-x86-stack-bof\src\dobetter_x86.c",
    "labs\02-x64-stack-bof\src\dobetter_x64.c"
)) {
    $p = Join-Path $RepoRoot $rel
    if (Test-Path $p) { Ok "Lab source: $rel" }
    else { Bad "Missing lab source: $rel" }
}

Write-Host ""
if ($failed -eq 0) {
    Write-Host "Verify finished with no hard failures." -ForegroundColor Green
    Write-Host "Still do a live check inside WinDBG: !load pykd  then  !mona"
    exit 0
}
else {
    Write-Host "Verify finished with $failed issue(s)." -ForegroundColor Yellow
    exit 1
}
