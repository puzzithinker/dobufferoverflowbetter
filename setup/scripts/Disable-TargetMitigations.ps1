<#
.SYNOPSIS
  Relax Windows Exploit Protection for a specific lab executable name.

.DESCRIPTION
  Intended ONLY for intentionally vulnerable training binaries inside a lab VM.
  Requires elevation. Uses the ProcessMitigations PowerShell module.

.PARAMETER Name
  Image name, e.g. dobetter_x86.exe

.PARAMETER KeepDEP
  Do not disable DEP (required for lab 05 ROP / NX teaching).

.EXAMPLE
  .\Disable-TargetMitigations.ps1 -Name dobetter_x86.exe
  .\Disable-TargetMitigations.ps1 -Name dobetter_rop_x86.exe -KeepDEP
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Name,
    [switch]$KeepDEP
)

$ErrorActionPreference = "Stop"

$isAdmin = ([Security.Principal.WindowsPrincipal] `
    [Security.Principal.WindowsIdentity]::GetCurrent()
).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    throw "Run this script from an elevated PowerShell (Run as administrator)."
}

Import-Module ProcessMitigations -ErrorAction Stop

Write-Host "[*] Applying mitigations relaxations for image name: $Name"
if ($KeepDEP) {
    Write-Host "[*] KeepDEP set — leaving DEP enabled (lab 05 ROP)"
}

# Policy names vary slightly by Windows build; try the full set then fall back.
$disable = @("SEHOP", "ForceRelocateImages", "BottomUp", "HighEntropy", "TerminateOnError")
if (-not $KeepDEP) {
    $disable = @("DEP") + $disable
}

try {
    Set-ProcessMitigation -Name $Name -Disable $disable
}
catch {
    Write-Host "[*] Full set failed ($($_.Exception.Message)); trying core set..."
    if ($KeepDEP) {
        Set-ProcessMitigation -Name $Name -Disable SEHOP, ForceRelocateImages, BottomUp, HighEntropy
    }
    else {
        Set-ProcessMitigation -Name $Name -Disable DEP, SEHOP, ForceRelocateImages, BottomUp, HighEntropy
    }
}

Write-Host "[+] Current policy:"
Get-ProcessMitigation -Name $Name | Format-List

Write-Host ""
Write-Host "Note: re-enable protections when you leave the lab. This weakens security for that image name."
