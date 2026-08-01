# Module 3 — Concepts + first exploit (x86)

## Goal

Understand stack-based saved return pointer overwrites, then complete
[labs/01-x86-stack-bof](../labs/01-x86-stack-bof/) with WinDBG + mona3 + Python 3.

## Concepts (read before or alongside the lab)

1. **Process memory** — code, data, heap, stack grow directions (high→low on x86/x64 Windows).
2. **Registers** — `EIP`/`ESP`/`EBP` (x86); later `RIP`/`RSP`/`RBP` (x64).
3. **`CALL` / `RET`** — return address pushed; `RET` pops to instruction pointer.
4. **Local buffers** — overflow past saved frame pointer into **saved return pointer (SRP)**.
5. **EIP control** — proof that you own the next instruction address.
6. **Bad characters** — bytes truncated or transformed by parsers (`\x00`, `\n`, …).
7. **JMP ESP trampoline** — ret to gadget that jumps to stack-hosted shellcode.
8. **Shellcode** — position-independent bytes; avoid bad chars; watch decoder stubs.

## Primary lab

Follow **[labs/01-x86-stack-bof/TUTORIAL.md](../labs/01-x86-stack-bof/TUTORIAL.md)** end-to-end.

Progressive scripts (write your own first; compare only if stuck):

| Script | Milestone |
|--------|-----------|
| `exploits/01_connect.py` | TCP hello |
| `exploits/02_crash.py` | Long line → crash |
| `exploits/03_pattern.py` | Cyclic pattern |
| `exploits/04_eip_control.py` | Exact EIP |
| `exploits/05_badchars.py` | Bad char hunt |
| `exploits/06_shellcode.py` | JMP ESP + payload |

## Companion free reading

- Corelan **Exploit Writing Tutorial Part 1** (classic text):  
  https://www.corelan.be/index.php/2009/07/19/exploit-writing-tutorial-part-1-stack-based-overflows/
- Corelan **Part 1 — The Video** (2026, Win11 x64 + WinDBG + mona):  
  https://www.corelan.be/index.php/2026/04/02/exploit-writing-tutorial-part-1-the-video/
- Original **dostackbufferoverflowgood** tutorial (Immunity-era narrative):  
  https://github.com/justinsteven/dostackbufferoverflowgood

## Checkpoint

- [ ] Can explain SRP overwrite with a sketch
- [ ] `!mona findmsp` / pattern offset matches your Python offset
- [ ] Land reliable EIP control (`BBBB` or similar)
- [ ] Execute lab shellcode with mitigations off

## Next

→ [04-vulnserver.md](04-vulnserver.md)
