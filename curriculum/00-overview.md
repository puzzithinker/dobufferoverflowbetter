# Module 0 — Overview and mindset

## Why stack overflows still matter in 2026

Classic stack buffer overflows are the **pedagogical backbone** of exploit
writing. Real-world user-mode Windows exploitation today often involves heap
corruption, type confusion, sandbox escapes, or heavy ROP — but those paths
assume you already understand:

- How control flow is stored (return addresses, function pointers)
- How debuggers show state at a crash
- How to turn “I control a register / instruction pointer” into “I run code”

This curriculum keeps the 90s mechanics, swaps in **2026 tools**, and is honest
about mitigations.

## Ethics

- Only exploit **lab software** or systems you own / have written permission to test.
- Snapshot VMs so you can recover from bad states.
- Treat shellcode and network listeners as **live ammo** — host-only networks help.

## How to study

1. **Type exploits yourself** — do not only read or paste solutions.
2. When the debugger lies to you (or you lie to yourself), **slow down**: draw the stack.
3. Prefer **one clean crash** over ten half-understood ones.
4. Keep a lab notebook: offsets, bad chars, gadget addresses, mona commands.

## Suggested weekly rhythm

| Day | Focus |
|-----|--------|
| 1 | Platform + tools (modules 1–2) |
| 2–3 | Lab 01 x86 through EIP control |
| 4 | Bad chars + JMP ESP + shellcode |
| 5 | Vulnserver practice |
| 6–7 | Lab 02 x64 + write notes |

## Next

→ [01-platform.md](01-platform.md)
