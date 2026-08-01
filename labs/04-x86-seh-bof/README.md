# Lab 04 — x86 SEH overwrite

Next step after classic saved-return-pointer control: overwrite the **Structured
Exception Handler** chain (nSEH + SEH) and divert execution when an exception
is raised.

| Item | Value |
|------|--------|
| Binary | `bin/dobetter_seh_x86.exe` |
| Port | **31340** |
| Bug | 512-byte buffer + adjacent SEH record; forced AV after copy |
| Gadget | `POP/POP/RET` (and mona `!mona seh`) |

## Prerequisites

- Lab 01 complete (EIP control, mona patterns, bad chars)
- WinDBG + mona3 on a Windows lab VM
- Exploit Protection relaxed for `dobetter_seh_x86.exe`

## Build

```bash
make -C labs/04-x86-seh-bof
# or MSVC x86: nmake /f Makefile.nmake
```

**Important:** this lab keeps SEH enabled (unlike lab 01’s optional `--no-seh`).

## Tutorial

→ [TUTORIAL.md](TUTORIAL.md)  
→ [TUTORIAL_EGGHUNTER.md](TUTORIAL_EGGHUNTER.md) (after basic SEH works)

## Safety

Isolated VM only. Forced null-pointer writes are intentional crash triggers.
