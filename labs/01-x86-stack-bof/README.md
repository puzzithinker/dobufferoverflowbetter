# Lab 01 — x86 stack buffer overflow

Modernized spiritual successor to [dostackbufferoverflowgood](https://github.com/justinsteven/dostackbufferoverflowgood):

| | Classic | This lab |
|--|---------|----------|
| Debugger | Immunity | **WinDBG + mona3** |
| Language | Python 2 | **Python 3** |
| JMP ESP | Huge stack frame side effect | **Explicit `gadget_jmp_esp`** |
| OS notes | Win7-era | **Win10/11 Exploit Protection** |

## Files

| Path | Role |
|------|------|
| [src/dobetter_x86.c](src/dobetter_x86.c) | Vulnerable TCP server |
| [TUTORIAL.md](TUTORIAL.md) | Step-by-step exploit path |
| [exploits/](exploits/) | Progressive Python 3 scripts |
| [solutions/](solutions/) | Reference offsets workflow (no spoilers for your binary base) |
| `Makefile` / `Makefile.nmake` | MinGW / MSVC builds |

## Build

```bash
make          # needs i686-w64-mingw32-gcc
```

```bat
nmake /f Makefile.nmake
```

Output: `bin/dobetter_x86.exe` — listen **TCP 31337**.

## Run (lab VM)

```powershell
# Optional mitigations relax
..\..\setup\scripts\Disable-TargetMitigations.ps1 -Name dobetter_x86.exe
.\bin\dobetter_x86.exe
```

Then follow [TUTORIAL.md](TUTORIAL.md).

## Bug (spoiler-light)

`do_response` builds `Hello` + name + `!!!\n` into a **128-byte** stack buffer with a **length-based** copy (no bounds check). Long lines overwrite the saved return pointer. Null bytes in the name are allowed (helps x86/x64 addresses); `\n` still ends the line.

Extra drills: [exercises/EXTRA.md](exercises/EXTRA.md). Gadget notes: [../shared/mona-gadgets.md](../shared/mona-gadgets.md).
