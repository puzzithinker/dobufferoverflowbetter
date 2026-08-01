# Module 6 — Linux reinforcement (optional)

Windows-first is fine. Linux challenges sharpen the same mental models with
different ABIs and tooling (`gdb`, `pwntools`, `checksec`).

## First-party lab (this repo)

Start here for a mitigations-off companion that mirrors the Windows labs:

→ **[labs/03-linux-stack-bof](../labs/03-linux-stack-bof/)** (`make linux`)

## Free external tracks

| Resource | Notes |
|----------|--------|
| [SEED Set-UID Buffer-Overflow Lab](https://seedsecuritylabs.org/Labs_20.04/Software/Buffer_Overflow_Setuid/) | Classic root exploit + protection evaluation (Ubuntu 20.04 materials) |
| [digitalandrew/overflowme](https://github.com/digitalandrew/overflowme) | Multi-level x86 stack lab with diagrams |
| [pwn.college Binary Exploitation](https://pwn.college/cse365-f2024/binary-exploitation/) | Structured challenges + lectures |
| [Day Zero Sec — Getting Started 2024](https://dayzerosec.com/blog/2024/07/11/getting-started-2024.html) | Curated free path (asm → pwn.college → ROP Emporium) |
| [exploit.education Phoenix](https://exploit.education/) | Protostar successor; “old-style” mitigations-off style |

## Suggested ordering if you do Linux in parallel

1. C + x86-64 asm basics
2. **This repo lab 03** (same mental model as Windows labs 01–02)
3. pwn.college fundamentals / first overflow
4. SEED or overflowme for deliberate stack smashing
5. Return to Windows lab 02 / Vulnserver with fresher intuition

## Checkpoint

- [ ] One Linux stack BOF to shell *or* flag without copy-paste
- [ ] Can contrast `checksec` output with our Windows “mitigations off” builds

## Next

→ [07-next-level.md](07-next-level.md)
