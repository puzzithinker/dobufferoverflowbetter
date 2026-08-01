# Module 7 — Mitigations and career depth

## Goal

Leave “mitigations off” kindergarten intentionally: understand what breaks your
lab exploits, then pick a structured path (free series or paid courses).

## What breaks classic stack BOFs

| Mitigation | Effect on your lab-01 exploit |
|------------|--------------------------------|
| **Stack cookies /GS** | Cookie check before `RET` → crash unless leaked/bypassed |
| **DEP / NX** | Stack not executable → need ROP or other code-reuse |
| **ASLR** | Module base random → fixed JMP ESP addresses die |
| **SafeSEH / SEHOP / CFG** | Harder SEH and indirect call abuse |
| **High-entropy ASLR / Force ASLR** | Even non-ASLR-aware binaries may relocate |

Your next skills: info leaks, ROP chain construction, egghunters, SEH overflows,
and reading real crash dumps.

## Before this module

- **[08-seh-and-beyond.md](08-seh-and-beyond.md)** / [labs/04](../labs/04-x86-seh-bof/)  
- **[09-dep-rop.md](09-dep-rop.md)** / [labs/05](../labs/05-x86-dep-rop/) ret2win at minimum  

Then SafeSEH/SEHOP/ASLR discussions will map onto labs you already broke.

## Free continuation

- Corelan exploit writing series (parts beyond #1) on https://www.corelan.be/
- mona3 ROP helpers (`!mona rop`, etc.) once DEP is on
- Vulnserver remaining commands ([../labs/vulnserver-practice/](../labs/vulnserver-practice/))
- Linux ROP Emporium after stack basics

## Structured / paid

| Course | Focus |
|--------|--------|
| [Corelan Stack-based Exploit Development](https://ccsecuritytraining.com/training/corelan-stack/) | Multi-day, mona.py from the author; stack → SEH → egghunters → ASLR/DEP/ROP → x64 intro |
| [OffSec EXP-301 (OSED)](https://www.offsec.com/courses/exp-301/) | Intermediate Windows user-mode: BOFs, custom shellcode, RE, DEP/ASLR bypass |

## Capstone ideas (self-directed)

1. Re-enable DEP only on lab 01; build a minimal ROP to `VirtualProtect` + shellcode.
2. Port one Vulnserver exploit to fully ASLR-aware using a leak (advanced).
3. Write a short blog/lab note teaching JMP ESP to a peer.

## You’re done with this repo when…

You can cold-start WinDBG + mona3, own EIP/RIP on a new toy binary, and explain
why the same payload fails with DEP or ASLR enabled.

Happy hacking — pop lab calc, not production `alert(1)`.
