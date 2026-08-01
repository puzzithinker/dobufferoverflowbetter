# Extra progressive exercises (lab 02)

## E1 — Null-byte awareness

Pack your JMP RSP address with `struct.pack('<Q', addr)` and print the hex
bytes. Count embedded `00`. Confirm length-based copy (this lab) still plants
all 8 bytes (`dqs rsp` / `dps` equivalent).

## E2 — Calling convention sketch

Before shellcode: draw where RCX/RDX/R8/R9 would matter if you called
`WinExec` manually. (You may still use msfvenom x64 exec payloads.)

## E3 — Alignment

After JMP RSP, single-step and note RSP alignment. Some x64 ABI-sensitive
shellcode wants 16-byte alignment before `call`; add NOPs if needed.

## E4 — WinDBGX only day

Do the entire lab using only WinDBGX (no Classic). Save mona Markdown output
from the working folder and review it in VS Code preview.
