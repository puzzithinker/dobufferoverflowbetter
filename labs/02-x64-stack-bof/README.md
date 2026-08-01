# Lab 02 — x64 stack buffer overflow

Same teaching bug as lab 01, built as a **64-bit** PE: control **RIP**, jump via
**JMP RSP**, run shellcode with DEP disabled.

## Files

| Path | Role |
|------|------|
| [src/dobetter_x64.c](src/dobetter_x64.c) | Vulnerable TCP server (port **31338**) |
| [TUTORIAL.md](TUTORIAL.md) | x64-focused walkthrough |
| [exploits/](exploits/) | Python 3 scripts (`struct.pack('<Q', ...)`) |
| `Makefile` / `Makefile.nmake` | MinGW-w64 / MSVC x64 |

## Build

```bash
make   # x86_64-w64-mingw32-gcc
```

```bat
nmake /f Makefile.nmake
```

## Run

```powershell
..\..\setup\scripts\Disable-TargetMitigations.ps1 -Name dobetter_x64.exe
.\bin\dobetter_x64.exe
```

Attach with **x64** WinDBG / WinDBGX, then `!mona jmp -r rsp`.

Extra drills: [exercises/EXTRA.md](exercises/EXTRA.md).
