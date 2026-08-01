# Setup: mona3 mirror, WinDBG helpers, Win11 notes

This directory prepares a **Windows 10/11 exploit-dev lab** to run mona3 with
WinDBG Classic or WinDBGX, and documents guest hardening trade-offs for learning.

## Contents

| Path | Purpose |
|------|---------|
| [win11-lab-notes.md](win11-lab-notes.md) | Full lab checklist (VM, mitigations, firewall, symbols) |
| [scripts/Mirror-Mona3.ps1](scripts/Mirror-Mona3.ps1) | Clone mona3 (+ optional CorelanTraining) into `tools/` |
| [scripts/Mirror-Mona3.sh](scripts/Mirror-Mona3.sh) | Same for Linux/macOS packaging hosts |
| [scripts/Install-Mona3.ps1](scripts/Install-Mona3.ps1) | Install centralized mona3 under `C:\Tools\mona3` |
| [scripts/Verify-MonaSetup.ps1](scripts/Verify-MonaSetup.ps1) | File/path sanity checks |
| [scripts/Disable-TargetMitigations.ps1](scripts/Disable-TargetMitigations.ps1) | Per-binary Exploit Protection relax for lab EXEs |
| [windbg/w.bat](windbg/w.bat) | WinDBG Classic x86 launcher with pykd + mona alias |
| [windbg/w64.bat](windbg/w64.bat) | WinDBG Classic x64 launcher |
| [windbg/windbgx-startup.txt](windbg/windbgx-startup.txt) | Paste into WinDBGX Startup settings |

## Recommended order on a fresh Win11 VM

1. Snapshot clean OS.
2. Install base tooling:
   - **Either** FLARE-VM  
   - **Or** Corelan `CorelanWin11VMInstall.ps1` + always `CorelanPyKDInstall.ps1`
3. Snapshot again.
4. Clone or copy this repo onto the VM.
5. Run:

   ```powershell
   cd C:\path\to\dobufferoverflowbetter
   Set-ExecutionPolicy -Scope Process Bypass
   .\setup\scripts\Mirror-Mona3.ps1
   .\setup\scripts\Install-Mona3.ps1
   .\setup\scripts\Verify-MonaSetup.ps1
   ```

6. Configure WinDBGX startup from `windbg/windbgx-startup.txt`, **or** copy
   `w.bat` / `w64.bat` next to `windbg.exe` and adjust paths.
7. Read [win11-lab-notes.md](win11-lab-notes.md).
8. Snapshot “lab ready.”

## mona3 centralized layout

Default install path:

```text
C:\Tools\mona3\mona.py
C:\Tools\mona3\windbglib.py
```

Alias (Python version must match your PyKD install — commonly 3.9 or 3.14):

```text
!load pykd
as !mona !py -3.9 C:\Tools\mona3\mona.py
```

Upstream docs: https://github.com/corelan/mona3

## Do not commit mirrored tools

`tools/mona3/` is gitignored. Always pull from upstream via the mirror scripts
so students get current mona3 without vendoring a 47k-line file in git history.
