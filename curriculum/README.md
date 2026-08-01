# Curriculum overview

Ordered modules for a free, self-paced path from “what is a stack?” to “I can
write a simple Windows stack BOF with WinDBG + mona3,” then into structured
practice and paid/formal depth.

## Module order

| # | Module | Goal | Time (rough) |
|---|--------|------|----------------|
| 0 | [00-overview.md](00-overview.md) | Mindset, ethics, how to study | 30 min |
| 1 | [01-platform.md](01-platform.md) | Isolated Win10/11 lab (FLARE / Corelan) | 2–4 h |
| 2 | [02-tools.md](02-tools.md) | WinDBG + mona3 install and muscle memory | 2–4 h |
| 3 | [03-concepts-and-first-lab.md](03-concepts-and-first-lab.md) | Stack mechanics + [labs/01-x86](../labs/01-x86-stack-bof/) | 1–3 days |
| 4 | [04-vulnserver.md](04-vulnserver.md) + [tutorial](04-vulnserver-tutorial.md) | Repeat on multi-bug target | 1–3 days |
| 5 | [05-x64-lab.md](05-x64-lab.md) | RIP control + [labs/02-x64](../labs/02-x64-stack-bof/) | 1–2 days |
| 6 | [06-linux.md](06-linux.md) + [labs/03](../labs/03-linux-stack-bof/) | Linux reinforcement | flexible |
| 7 | [08-seh-and-beyond.md](08-seh-and-beyond.md) + [labs/04](../labs/04-x86-seh-bof/) | SEH + [egghunter tutorial](../labs/04-x86-seh-bof/TUTORIAL_EGGHUNTER.md) | 1–2 days |
| 8 | [09-dep-rop.md](09-dep-rop.md) + labs [05](../labs/05-x86-dep-rop/)/[06](../labs/06-x64-dep-rop/) | DEP on, ret2win (x86 + x64) | 1–2 days |
| 9 | [07-next-level.md](07-next-level.md) | Courses, ASLR, modern Windows | ongoing |
| — | [resources.md](resources.md) | Master link list | reference |

## Learning outcomes

After modules 0–5 you should be able to:

1. Explain process memory layout, the stack, `CALL`/`RET`, and saved return pointers.
2. Attach WinDBG to a vulnerable server, set breakpoints, and read registers/stack.
3. Use mona3 for patterns, offsets, bad chars, and JMP ESP/RSP discovery.
4. Build a Python 3 exploit that crashes → controls EIP/RIP → lands shellcode (mitigations off).
5. State what ASLR, DEP/NX, and stack cookies change — and why this course disables them first.

## Prerequisites

- Comfortable with basic C, TCP sockets, and a shell
- Willing to read assembly (x86 first, then x64)
- A machine that can run a Windows VM (16+ GB host RAM recommended)

No prior exploit development required.
