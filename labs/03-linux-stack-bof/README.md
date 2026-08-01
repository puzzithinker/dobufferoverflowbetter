# Lab 03 — Linux stack buffer overflow (mitigations off)

Same pedagogical bug as the Windows labs (unsafe Hello+name copy into 128-byte
stack buffer), as a **native Linux ELF** for gdb / pwntools practice.

| Item | Value |
|------|--------|
| Binary | `bin/dobetter_linux` |
| Interactive | stdin lines |
| TCP | `--serve [port]` (default **31339**) |
| Arch | x86-64 host (primary); i386 if you build `-m32` |

## Build

```bash
make -C labs/03-linux-stack-bof
# flags: -fno-stack-protector -z execstack -no-pie

# Optional NX twin (non-exec stack) for after lab 05 concepts:
make -C labs/03-linux-stack-bof -f Makefile.nx
```

## Run

```bash
./bin/dobetter_linux                 # interactive
./bin/dobetter_linux --serve 31339   # TCP
```

## Tutorial

See [TUTORIAL.md](TUTORIAL.md). Extra: [exercises/EXTRA.md](exercises/EXTRA.md).

## Tools

- `gdb` + [pwndbg](https://github.com/pwndbg/pwndbg) / gef (optional)
- `python3` + optional `pwntools`
- Progressive scripts `exploits/01` … `06` (incl. `05_badchars.py`)
