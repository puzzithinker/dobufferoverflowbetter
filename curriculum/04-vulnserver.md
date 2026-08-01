# Module 4 — Vulnserver (repetition)

## Goal

Transfer lab-01 skills to a multi-command intentionally vulnerable Windows
server so the methodology becomes muscle memory.

## Target

- Source: [stephenbradshaw/vulnserver](https://github.com/stephenbradshaw/vulnserver)
- Default listen port: **9999**
- Multiple distinct overflow-prone commands (TRUN, GMON, GTER, HTER, …)

## Method (same every time)

1. Fingerprint the command / fuzz for crash
2. Attach WinDBG; note exception and registers
3. Cyclic pattern → offset to EIP (or SEH chain later)
4. Bad character pass
5. Find redirect gadget (`!mona jmp -r esp` etc.)
6. Generate shellcode (msfvenom / custom) excluding bad chars
7. Stabilize and document

## In-repo playbook

Step-by-step methodology (fuzz → offset → bad chars → gadget → shellcode) without
full spoilers: **[04-vulnserver-tutorial.md](04-vulnserver-tutorial.md)**.

Progressive Python skeletons (fill your own offsets):
**[../labs/vulnserver-practice/](../labs/vulnserver-practice/)**.

## Walkthroughs (external)

Use walkthroughs as **hints**, not scripts to paste blindly. Example:

- Phil Keeble Vulnserver series (WinDBG + mona style):  
  https://philkeeble.com/exploitation/windows/Vulnserver-Walkthrough-Part-1/

## Win11 tips

- Disable Exploit Protection for `vulnserver.exe` (see [../setup/win11-lab-notes.md](../setup/win11-lab-notes.md)).
- Allow TCP 9999 in the guest firewall or disable firewall in the lab VM.
- Prefer host-only networking; attack from a second VM or WSL carefully.

## Checkpoint

- [ ] At least two different Vulnserver commands exploited cleanly
- [ ] Notes include offsets, bad chars, gadgets per command
- [ ] You can redo one exploit cold the next day without the walkthrough

## Next

→ [05-x64-lab.md](05-x64-lab.md)
