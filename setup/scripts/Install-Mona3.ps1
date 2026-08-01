<#
.SYNOPSIS
  Install mona3 into a centralized location (default C:\Tools\mona3).

.DESCRIPTION
  Copies mona.py and windbglib.py from the local tools/mona3 mirror (or clones
  if missing). Prints WinDBG alias instructions. Prefer CorelanPyKDInstall.ps1
  for Python/PyKD/keystone dependencies.

.PARAMETER InstallDir
  Destination directory (default: C:\Tools\mona3)

.PARAMETER PythonTag
  py launcher tag used in the suggested alias (default: 3.9)
#>
[CmdletBinding()]
param(
    [string]$InstallDir = "C:\Tools\mona3",
    [string]$PythonTag = "3.9",
    [string]$RepoRoot = ""
)

$ErrorActionPreference = "Stop"

if (-not $RepoRoot) {
    $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
}

$mirror = Join-Path $RepoRoot "tools\mona3"
if (-not (Test-Path (Join-Path $mirror "mona.py"))) {
    Write-Host "[*] Mirror missing; running Mirror-Mona3.ps1"
    & (Join-Path $PSScriptRoot "Mirror-Mona3.ps1") -RepoRoot $RepoRoot
}

New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null

Copy-Item -Force (Join-Path $mirror "mona.py") (Join-Path $InstallDir "mona.py")
Copy-Item -Force (Join-Path $mirror "windbglib.py") (Join-Path $InstallDir "windbglib.py")

# Optional extras if present in mirror
foreach ($extra in @("README.md", "mona_releasenotes.txt")) {
    $src = Join-Path $mirror $extra
    if (Test-Path $src) {
        Copy-Item -Force $src (Join-Path $InstallDir $extra)
    }
}

$mona = Join-Path $InstallDir "mona.py"
$first = (Get-Content $mona -TotalCount 5 | Out-String)
if ($first -match "<html|<!DOCTYPE") {
    throw "Installed mona.py looks like HTML. Re-mirror from GitHub raw/git clone."
}

$work = "C:\mona-work"
New-Item -ItemType Directory -Force -Path $work | Out-Null

Write-Host ""
Write-Host "[+] mona3 installed to $InstallDir"
Write-Host ""
Write-Host "WinDBG / WinDBGX (after attaching to a process):"
Write-Host "  !load pykd"
Write-Host "  as !mona !py -$PythonTag $InstallDir\mona.py"
Write-Host "  !mona config -set workingfolder $work\%p"
Write-Host "  !mona"
Write-Host ""
Write-Host "If Python $PythonTag is not available, run !pykd.info and adjust -$PythonTag."
Write-Host "Dependencies: run CorelanPyKDInstall.ps1 from corelan/CorelanTraining."
Write-Host ""
Write-Host "WinDBGX startup snippet: setup\windbg\windbgx-startup.txt"
Write-Host "Classic launchers:       setup\windbg\w.bat and w64.bat"
