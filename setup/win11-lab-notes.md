# Windows 11 lab notes (exploit learning)

Practical checklist for running **intentionally vulnerable** binaries with
**WinDBG + mona3** on Windows 10/11 in 2026.

## 1. VM topology

```text
[Host]
  ├─ Lab-Win11  (host-only / isolated)  ← vulnerable servers + WinDBG
  └─ optional Attacker-Linux            ← msfvenom, Python exploits
```

- Prefer **host-only** or custom isolated vSwitch after tooling install.
- Snapshot: **clean OS** → **tools installed** → **lab ready**.
- Username without spaces helps FLARE-VM and some scripts.

## 2. What to install

### Minimum

| Component | How |
|-----------|-----|
| WinDbg | `winget install Microsoft.WinDbg` or Windows SDK Debugging Tools |
| Python 3 (32- and 64-bit as required by PyKD) | Corelan `CorelanPyKDInstall.ps1` recommended |
| PyKD + pykd-ext | Same script |
| mona3 | This repo `Install-Mona3.ps1` |
| Build tools | **MSVC:** Visual Studio Build Tools → “Desktop development with C++”; open **x86** vs **x64** Native Tools prompt for each lab. **MinGW:** MSYS2 mingw32/mingw64 shells (see [BUILDING.md](../BUILDING.md)). Or copy prebuilt PE from a Linux `make` host. |
| Python 3 for exploits | Store / winget; use venv if you like |

### Comfortable

- FLARE-VM (x64dbg, Ghidra, NASM, scdbg, …)
- Visual Studio Code
- 7-Zip
- Optional: Metasploit on attacker VM for `msfvenom`

### Symbols

Set a system-wide symbol path (Corelan scripts often do this):

```text
SRV*C:\Symbols*https://msdl.microsoft.com/download/symbols
```

PowerShell (elevated):

```powershell
[Environment]::SetEnvironmentVariable(
  "_NT_SYMBOL_PATH",
  "SRV*C:\Symbols*https://msdl.microsoft.com/download/symbols",
  "Machine")
```

## 3. mona3 + WinDBG quick reference

After attach:

```text
!load pykd
!pykd.info
as !mona !py -3.9 C:\Tools\mona3\mona.py
!mona config -set workingfolder C:\mona-work\%p
!mona
```

If `!py -3.9` fails, try `-3.14` or whatever `!pykd.info` lists.

WinDBGX: File → Settings → Debugging settings → Startup → paste
[windbg/windbgx-startup.txt](windbg/windbgx-startup.txt).

Classic: use [windbg/w.bat](windbg/w.bat) / [w64.bat](windbg/w64.bat).

## 4. Mitigations: compiler vs OS

Lab binaries in this repo are built with **no GS cookie, no ASLR, no DEP** at
link time (when built with the provided flags). Windows 11 can still apply
**Exploit Protection** policies.

### Per-binary (preferred)

```powershell
# From repo root, elevated:
.\setup\scripts\Disable-TargetMitigations.ps1 -Name dobetter_x86.exe
.\setup\scripts\Disable-TargetMitigations.ps1 -Name dobetter_x64.exe
```

Or manually:

```powershell
Set-ProcessMitigation -Name dobetter_x86.exe -Disable DEP,BottomUp,HighEntropy,SEHOP,ForceRelocateImages
Get-ProcessMitigation -Name dobetter_x86.exe
```

Windows Security UI: **App & browser control → Exploit protection → Program settings**.

### System NX (last resort, lab VMs only)

Legacy `bcdedit` approaches exist (`nx AlwaysOff`) but are blunt and require
reboot. Prefer per-app mitigations on modern Windows.

### Firewall

```powershell
New-NetFirewallRule -DisplayName "dobetter labs" -Direction Inbound -Protocol TCP `
  -LocalPort 31337,31338,31340,31341,31342 -Action Allow -Profile Any
```

Or disable the firewall entirely inside a disposable lab VM.

## 5. Defender / SmartScreen

FLARE and many RE labs disable real-time protection because signatures flag
shellcode tools and crashers. If you keep Defender on:

- Add exclusions for `C:\Tools`, your repo path, and mona working folder
- Expect false positives on student shellcode files

**Never** weaken Defender on a host you use for email/banking.

## 6. Attach workflow for TCP labs

1. Start `dobetter_x86.exe` (console stays open, listens).
2. WinDBG → File → Attach to process → select the process.
3. Load pykd/mona; set working folder.
4. Optional: break on module load / set `bp dobetter_x86!do_response` if symbols exist.
5. From attacker: run Python exploit against guest IP (or `127.0.0.1` if local).
6. On crash: `r`, `dps esp`, `!mona findmsp`.

If you **launch** under the debugger instead of attach, use a non-invasive
strategy so threads accepting sockets still run (`g` after initial break).

## 7. Architecture matching

| Target | Debugger | mona notes |
|--------|----------|------------|
| x86 lab | WinDBG x86 or WinDBGX | `!mona jmp -r esp` |
| x64 lab | WinDBG x64 or WinDBGX | `!mona jmp -r rsp`; 8-byte addresses |

## 8. Common failures

| Symptom | Likely cause |
|---------|----------------|
| `!load pykd` fails | pykd-ext not installed / wrong extension path |
| `import pykd` errors | Python arch ≠ debuggee arch |
| mona “file is HTML” | Bad download of mona.py (save raw from GitHub) |
| EIP never controlled | Offset wrong; newline/`\0` truncated payload; wrong thread |
| JMP ESP works, shellcode dies | Bad chars; DEP still on; stack clobbered |
| Fixed address fails after reboot | ASLR still enabled for that binary |

## 9. Safety reminder

These notes intentionally reduce security **inside a lab guest**. Keep the VM
isolated, snapshotted, and free of personal credentials.
