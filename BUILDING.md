# Building the labs

How to produce the intentionally vulnerable binaries on Linux, macOS, and Windows.

## Quick matrix

| Host | 01 x86 | 02 x64 | 03 Linux | 04 SEH | 05 ROP x86 | 06 ROP x64 |
|------|--------|--------|----------|--------|------------|------------|
| Linux MinGW | `make -C labs/01-…` | `make -C labs/02-…` | `make -C labs/03-…` | `make -C labs/04-…` | `make -C labs/05-…` | `make -C labs/06-…` |
| Repo root | `make` | (in `make`) | `make linux` | `make seh` | `make rop` | `make rop64` |
| MSVC | x86 nmake | x64 nmake | n/a | x86 | x86 NX on | x64 NX on |

Outputs land in each lab’s `bin/` directory (gitignored). Rebuild after pulling.

## Linux / macOS — MinGW-w64 (Windows PE targets)

### Install toolchain

```bash
# Debian / Ubuntu
sudo apt update
sudo apt install -y mingw-w64 make

# Verify
i686-w64-mingw32-gcc --version
x86_64-w64-mingw32-gcc --version
```

### Build

```bash
# From repo root — both Windows labs
make

# Or individually
make -C labs/01-x86-stack-bof
make -C labs/02-x64-stack-bof
```

### Important: do not use host `CC`

Makefiles use **`MINGW_CC`**, not `CC`. Many environments export `CC=cc`, which
would otherwise try to compile Windows sources with the host compiler and fail
on `winsock2.h`.

Override only when your triplet differs:

```bash
make -C labs/01-x86-stack-bof MINGW_CC=i686-w64-mingw32-gcc
make -C labs/02-x64-stack-bof MINGW_CC=x86_64-w64-mingw32-gcc
```

### Expected `file` output

```text
dobetter_x86.exe: PE32 executable (console) Intel 80386, for MS Windows
dobetter_x64.exe: PE32+ executable (console) x86-64, for MS Windows
```

Mitigation flags (MinGW labs 01–02): `-fno-stack-protector`, `-Wl,--disable-dynamicbase`,
`-Wl,--disable-nxcompat` (and `--no-seh` when supported on i686 for lab 01).

**Lab 04 (SEH)** must **not** use `--no-seh` — the SEH chain has to work.

**Lab 05 (ROP)** uses `-Wl,--nxcompat` (DEP **on**). Confirm with
`objdump -x … | grep NX_COMPAT`.

**Lab 03 NX variant:** `make -C labs/03-linux-stack-bof -f Makefile.nx`

## Windows — MSVC

1. Install **Visual Studio Build Tools** with the C++ workload (or full VS).
2. Open the matching **Developer Command Prompt**:
   - Lab 01: **x86 Native Tools Command Prompt**
   - Lab 02: **x64 Native Tools Command Prompt**
3. Build:

```bat
cd labs\01-x86-stack-bof
nmake /f Makefile.nmake

cd ..\02-x64-stack-bof
nmake /f Makefile.nmake
```

Flags: `/GS-`, `/DYNAMICBASE:NO`, `/NXCOMPAT:NO` (x86 also `/SAFESEH:NO`).

## Windows — MSYS2 MinGW

```bash
# In the MINGW32 shell (lab 01):
pacman -S mingw-w64-i686-gcc make
cd /c/path/to/dobufferoverflowbetter/labs/01-x86-stack-bof
make MINGW_CC=gcc

# In the MINGW64 shell (lab 02):
pacman -S mingw-w64-x86_64-gcc make
cd /c/path/to/dobufferoverflowbetter/labs/02-x64-stack-bof
make MINGW_CC=gcc
```

Use a **Unix-like shell** (MSYS2, Git Bash) for the MinGW Makefiles (`mkdir -p`,
`rm -rf`). Pure `cmd.exe` should use `nmake` instead.

## Linux ELF companion (lab 03)

```bash
make -C labs/03-linux-stack-bof
# or: make linux
./labs/03-linux-stack-bof/bin/dobetter_linux
```

Built with `-fno-stack-protector -z execstack -no-pie` (mitigations off).

## Compiler differences you must re-measure

| Topic | Why it matters |
|-------|----------------|
| EIP/RIP **offset** | Frame layout differs (MSVC vs MinGW, debug vs release) |
| **Gadget address** | Image base + section placement differ |
| Gadget **form** | MinGW: named `gadget_jmp_esp` / `gadget_jmp_rsp` in `.text`. MSVC x64: bytes in a custom executable `gadgets` section — still findable with `!mona jmp -r rsp` if executable |
| Startup **hint** printf | Convenience only; always prefer mona |

Never copy an offset from a write-up for a different binary build.

## Smoke-check gadgets (Linux host)

```bash
i686-w64-mingw32-objdump -d labs/01-x86-stack-bof/bin/dobetter_x86.exe | grep -A1 gadget_jmp
x86_64-w64-mingw32-objdump -d labs/02-x64-stack-bof/bin/dobetter_x64.exe | grep -A1 gadget_jmp
```

You should see `jmp *%esp` / `jmp *%rsp` (`ff e4`).
