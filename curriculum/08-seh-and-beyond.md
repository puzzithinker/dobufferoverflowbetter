# Module 8 — SEH, egghunters, and the road to DEP

## Goal

Move past direct EIP control into **exception-driven** redirection (SEH), then
understand where **egghunters** and **DEP/ROP** fit.

## First-party lab

→ **[labs/04-x86-seh-bof](../labs/04-x86-seh-bof/)** (port 31340)

Complete lab 01 before this module.

## Concepts

| Term | Meaning |
|------|---------|
| SEH chain | Linked list of exception registration records (`FS:[0]` on x86) |
| nSEH | “Next” pointer field (often short-jmp in exploits) |
| SEH | Handler pointer field (often POP/POP/RET in classic exploits) |
| SafeSEH | Module table of allowed handlers — breaks arbitrary P/P/R |
| SEHOP | Validates chain integrity toward `0xFFFFFFFF` |
| Egghunter | Tiny stub that searches memory for an “egg” + full payload |

## Suggested order

1. Lab 04 tutorial through INT3 after short jump  
2. [TUTORIAL_EGGHUNTER.md](../labs/04-x86-seh-bof/TUTORIAL_EGGHUNTER.md)  
3. One Vulnserver command that is SEH-oriented ([practice pack](../labs/vulnserver-practice/))  
4. [09-dep-rop.md](09-dep-rop.md) / lab 05 (DEP on)  
5. [07-next-level.md](07-next-level.md) for courses / ASLR  

## Free reading

- Corelan exploit writing series (SEH-focused parts) — https://www.corelan.be/  
- mona: `!mona seh`, `!mona egg`, `!mona findmsp`  

## Checkpoint

- [ ] Can explain why short jump is 4 bytes in nSEH  
- [ ] Can find P/P/R with mona and validate under WinDBG  
- [ ] Can state what SafeSEH/SEHOP change  
