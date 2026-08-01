# Module 9 — DEP and ROP intro

## Goal

Experience **why** lab-01 stack shellcode fails under NX, then land a minimal
**code-reuse** exploit (ret2win) and understand the `VirtualProtect` ROP shape.

## Labs

| Lab | Port | Focus |
|-----|------|--------|
| [05-x86-dep-rop](../labs/05-x86-dep-rop/) | 31341 | x86 ret2win + VP layout notes |
| [06-x64-dep-rop](../labs/06-x64-dep-rop/) | 31342 | x64 ret2win |
| [07-linux-nx-ret2win](../labs/07-linux-nx-ret2win/) | 31343 | Linux NX ret2win (no Windows needed) |

Offsets: [../labs/shared/mingw-offset-appendix.md](../labs/shared/mingw-offset-appendix.md) (`make meta`).

Prerequisites: lab 01 (and preferably 04 + egghunter reading).

## Key ideas

| Idea | Takeaway |
|------|----------|
| DEP/NX | Data pages not executable |
| ROP | Reuse existing executable gadgets ending in `ret` |
| ret2win | Smallest useful chain: one function address |
| VirtualProtect | API to re-permission memory, then run shellcode |
| ASLR still off here | Next pain after DEP |

## Linux twin

```bash
make -C labs/03-linux-stack-bof -f Makefile.nx
readelf -l labs/03-linux-stack-bof/bin/dobetter_linux_nx | grep GNU_STACK
```

Expect non-executable stack; stack shellcode fails.

## Checkpoint

- [ ] ret2win on lab 05  
- [ ] Can sketch VP stack layout on paper  
- [ ] Know egghunter ≠ DEP bypass  

## Next

[07-next-level.md](07-next-level.md) — full courses, ASLR, modern Windows.
