<#
.SYNOPSIS
  Clone mona3 (and optionally CorelanTraining) into this repo's tools/ directory.

.DESCRIPTION
  Does not vendor mona into git — populates tools/mona3 for offline install via
  Install-Mona3.ps1. Re-run to update (git pull if already cloned).

.PARAMETER AlsoCorelanTraining
  Also mirror https://github.com/corelan/CorelanTraining

.PARAMETER RepoRoot
  Override repo root (default: two levels above this script).
#>
[CmdletBinding()]
param(
    [switch]$AlsoCorelanTraining,
    [string]$RepoRoot = ""
)

$ErrorActionPreference = "Stop"

if (-not $RepoRoot) {
    $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
}

$tools = Join-Path $RepoRoot "tools"
New-Item -ItemType Directory -Force -Path $tools | Out-Null

function Sync-GitRepo {
    param(
        [string]$Url,
        [string]$Dest
    )
    if (Test-Path (Join-Path $Dest ".git")) {
        Write-Host "[*] Updating $Dest"
        git -C $Dest pull --ff-only
    }
    elseif (Test-Path $Dest) {
        throw "Path exists but is not a git repo: $Dest"
    }
    else {
        Write-Host "[*] Cloning $Url -> $Dest"
        git clone --depth 1 $Url $Dest
    }
}

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw "git is required on PATH"
}

Sync-GitRepo -Url "https://github.com/corelan/mona3.git" -Dest (Join-Path $tools "mona3")

if ($AlsoCorelanTraining) {
    Sync-GitRepo -Url "https://github.com/corelan/CorelanTraining.git" -Dest (Join-Path $tools "CorelanTraining")
}

$mona = Join-Path $tools "mona3\mona.py"
$wlib = Join-Path $tools "mona3\windbglib.py"
foreach ($f in @($mona, $wlib)) {
    if (-not (Test-Path $f)) { throw "Missing expected file: $f" }
    $head = Get-Content -Path $f -TotalCount 1 -ErrorAction SilentlyContinue
    if ($head -match "<!DOCTYPE|<html") {
        throw "File looks like HTML, not Python: $f"
    }
}

Write-Host "[+] mona3 mirror ready at $(Join-Path $tools 'mona3')"
Write-Host "    Next: .\setup\scripts\Install-Mona3.ps1"
