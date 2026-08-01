# Module 1 — Lab platform

## Goal

A disposable Windows 10/11 virtual machine with debuggers, Python, and network
isolation suitable for intentional vulnerability practice.

## Recommended setups

### Option A — FLARE-VM (broad RE toolkit)

- Project: [mandiant/flare-vm](https://github.com/mandiant/flare-vm)
- Requirements (from upstream): Windows 10+, PowerShell 5+, ~60 GB disk, ≥2 GB RAM,
  username without spaces, Internet during install
- Pre-install: disable Tamper Protection / anti-malware / Windows Update as documented
- Workflow: snapshot clean guest → install FLARE-VM → **host-only** network → snapshot again
- Defaults often include WinDbg, x64dbg, Ghidra, NASM, VC tools, shellcode helpers

Learning Hub notes: [mandiant/flare-learning-hub](https://github.com/mandiant/flare-learning-hub)

### Option B — Corelan Win11 training VM scripts

- [corelan/CorelanTraining](https://github.com/corelan/CorelanTraining)
- `CorelanWin11VMInstall.ps1` — debugger/Python/mona-oriented setup
- `CorelanPyKDInstall.ps1` — PyKD + keystone for mona3

This path is excellent if your primary goal is **exploit writing**, not full malware analysis.

### Option C — Minimal custom VM

1. Windows 11 Pro/Education evaluation VM
2. Install WinDbg (`winget install Microsoft.WinDbg`)
3. Install Visual Studio Build Tools or MinGW-w64
4. Run this repo’s [setup/](../setup/) scripts for mona3

## Hardening *the host*, weakening *the guest*

| Host | Guest (lab) |
|------|-------------|
| Keep patches and AV on | Snapshots; may disable AV for lab tools |
| No bridged attack traffic to LAN | Prefer **host-only** / isolated vSwitch |
| Your real files stay off the guest | Shared folders only if you accept the risk |

## DEP / Exploit Protection (preview)

Modern Windows can force DEP and other mitigations even on “opt-out” binaries.
Full notes: [../setup/win11-lab-notes.md](../setup/win11-lab-notes.md).

Short version for later labs:

```powershell
# Example: relax mitigations for a specific lab binary (elevated)
Set-ProcessMitigation -Name dobetter_x86.exe -Disable DEP, BottomUp, HighEntropy, SEHOP, ForceRelocateImages
```

Always prefer **per-binary** overrides over system-wide “turn everything off.”

## Checkpoint

- [ ] Clean snapshot exists
- [ ] Post-tooling snapshot exists
- [ ] Guest can reach attacker box (or localhost) only as intended
- [ ] You can open WinDBG and attach to notepad as a dry run

## Next

→ [02-tools.md](02-tools.md)
