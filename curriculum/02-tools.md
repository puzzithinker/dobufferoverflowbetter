# Module 2 — WinDBG and mona3

## Goal

Install and verify **WinDBG Classic / WinDBGX**, **PyKD**, and **mona3** so that
`!mona` works in both 32-bit and 64-bit sessions.

## Official / primary sources

| Resource | URL |
|----------|-----|
| mona3 | https://github.com/corelan/mona3 |
| mona v3 announcement | https://www.corelan.be/index.php/2026/05/01/mona-v3-released/ |
| WinDBG fundamentals (Corelan) | https://www.corelan.be/index.php/2026/03/23/debugging-windbg-windbgx-fundamentals/ |
| Microsoft WinDbg docs | https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/ |
| CorelanTraining scripts | https://github.com/corelan/CorelanTraining |

Session layout reference (text): [../docs/windbg-session-map.md](../docs/windbg-session-map.md).  
Gadget discovery notes: [../labs/shared/mona-gadgets.md](../labs/shared/mona-gadgets.md).

## Use this repo’s setup

From the repo root on the Windows lab VM (elevated PowerShell):

```powershell
.\setup\scripts\Mirror-Mona3.ps1          # clone mona3 into tools/mona3
.\setup\scripts\Install-Mona3.ps1         # copy to C:\Tools\mona3 + tips
.\setup\scripts\Verify-MonaSetup.ps1      # sanity checks
```

Read the full procedure: [../setup/README.md](../setup/README.md) and
[../setup/win11-lab-notes.md](../setup/win11-lab-notes.md).

Recommended dependency install (upstream):

```powershell
# From CorelanTraining (or your mirror under tools/CorelanTraining)
powershell -ExecutionPolicy Bypass -File .\CorelanPyKDInstall.ps1
```

## Everyday mona commands (lab set)

Run after attaching to a process and loading pykd:

```text
!load pykd
as !mona !py -3.9 C:\Tools\mona3\mona.py
!mona config -set workingfolder C:\mona-work\%p
!mona
!mona pattern_create 600
!mona pattern_offset <value>
!mona findmsp
!mona bytearray -cpb "\x00\x0a"
!mona compare -f C:\mona-work\bytearray.bin -a <addr>
!mona jmp -r esp -cpb "\x00\x0a"
!mona jmp -r rsp -cpb "\x00\x0a"    # x64 sessions
```

Python version flags (`-3.9`, `-3.14`, …) must match what PyKD install put on the VM.
See mona3 README for details.

## WinDBG muscle memory

| Command | Meaning |
|---------|---------|
| `g` | Go / continue |
| `bp module!symbol` | Breakpoint |
| `bl` / `bc *` | List / clear breakpoints |
| `r` | Registers |
| `dps rsp L20` / `dps esp L20` | Stack dwords/qwords with symbols |
| `u rip` / `u eip` | Disassemble at instruction pointer |
| `.restart` | Restart process (if launched under debugger) |
| `~*k` | Backtraces for all threads |

Use architecture-matched WinDBG Classic (`Debuggers\x86` vs `x64`) or let WinDBGX auto-select.

## Checkpoint

- [ ] `!load pykd` succeeds
- [ ] `!mona` prints the command table
- [ ] Working folder creates Markdown/output files
- [ ] You can attach to a 32-bit and a 64-bit process once each

## Next

→ [03-concepts-and-first-lab.md](03-concepts-and-first-lab.md)
