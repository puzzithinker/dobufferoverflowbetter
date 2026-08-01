# Labs

Intentionally vulnerable teaching targets. **Run only in isolated lab VMs.**

| Lab | Binary | Port | Focus |
|-----|--------|------|--------|
| [01-x86-stack-bof](01-x86-stack-bof/) | `dobetter_x86.exe` | 31337 | Classic SRP → EIP → JMP ESP → shellcode |
| [02-x64-stack-bof](02-x64-stack-bof/) | `dobetter_x64.exe` | 31338 | RIP control → JMP RSP → shellcode |
| [03-linux-stack-bof](03-linux-stack-bof/) | `dobetter_linux` | 31339 (optional TCP) | Same bug on Linux (gdb/pwntools) |
| [04-x86-seh-bof](04-x86-seh-bof/) | `dobetter_seh_x86.exe` | 31340 | nSEH/SEH → POP/POP/RET → shellcode |
| [05-x86-dep-rop](05-x86-dep-rop/) | `dobetter_rop_x86.exe` | 31341 | DEP on → ret2win / ROP |
| [06-x64-dep-rop](06-x64-dep-rop/) | `dobetter_rop_x64.exe` | 31342 | x64 DEP on → ret2win |
| [07-linux-nx-ret2win](07-linux-nx-ret2win/) | `dobetter_linux_ret2win` | 31343 | Linux NX ret2win |
| [08-aarch64-stack-bof](08-aarch64-stack-bof/) | `dobetter_aarch64` | 31344 | AArch64 stack BOF |
| [vulnserver-practice](vulnserver-practice/) | (upstream Vulnserver) | 9999 | Multi-command practice skeletons |

Shared notes: [shared/](shared/) (incl. [mona-gadgets.md](shared/mona-gadgets.md)).  
Diagrams: [../docs/images/](../docs/images/).  
Extra exercises: each lab’s `exercises/EXTRA.md` where present.

## Design principles (“do buffer overflow better”)

1. **WinDBG + mona3 first** — not Immunity-only.
2. **Python 3** exploit scripts.
3. **Explicit teaching gadgets** (`JMP ESP` / `JMP RSP`) so mona has a reliable target without opaque stack-size tricks.
4. **Mitigations off** at build time; document OS-level Exploit Protection separately.
5. **Progressive scripts** — connect → crash → pattern → control → bad chars → shellcode.
6. **Solutions optional** — try the tutorial before opening `solutions/`.

## Build (summary)

```bash
# From repo root (Linux/macOS + MinGW-w64)
make           # labs 01 + 02
make linux     # lab 03
make seh       # lab 04
make rop       # lab 05
make rop64     # lab 06
```

Offset cheat-sheet (re-measure always): [shared/mingw-offset-appendix.md](shared/mingw-offset-appendix.md).

```powershell
# Windows MSVC — use the matching Developer Command Prompt (x86 vs x64)
cd labs\01-x86-stack-bof
nmake /f Makefile.nmake
```

Full matrix (MSVC, MSYS2, flags, portability): [../BUILDING.md](../BUILDING.md).
