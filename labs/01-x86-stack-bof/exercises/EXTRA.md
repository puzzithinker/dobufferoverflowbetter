# Extra progressive exercises (lab 01)

Complete the main `TUTORIAL.md` first. These stretch the same binary.

## E1 — Offset without mona

Use only Python `cyclic` / `cyclic_find` from `exploits/common.py` (or your own)
to recover EIP offset. Cross-check once with `!mona findmsp`.

## E2 — Dual markers

Overwrite EIP with `BBBB` and the next four stack bytes with `CCCC`. Confirm
with `dps esp` that ESP points at `CCCC` after the faulting RET setup. This
prepares you for shellcode placement.

## E3 — Bad-char automation

Write a script that binary-searches bad characters (send half the alphabet,
see where mona compare / dump diverges). Document final bad set.

## E4 — INT3 then calc

Land `0xCC` first under WinDBG (debugger breaks). Only then swap in msfvenom
`windows/exec` / `calc`. Note encoder stub size vs free space after EIP.

## E5 — Restart discipline

Time yourself: from process not running → attach → mona loaded → EIP control
confirmed, cold. Aim under 10 minutes. Capture your personal checklist.

## E6 — MinGW vs MSVC

If you have both builds, compare offsets and gadget addresses in a table.
Explain *why* they differ without looking at assembly first, then verify.
