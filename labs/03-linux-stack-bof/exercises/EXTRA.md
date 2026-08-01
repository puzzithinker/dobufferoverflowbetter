# Extra exercises — Linux lab

## E1 — checksec fluency

Record canary / NX / PIE / RELRO for `dobetter_linux`. Rebuild *without*
`-z execstack` and note which step of the tutorial breaks first.

## E2 — tcp mode

```bash
./bin/dobetter_linux --serve 31339 &
MODE=tcp TARGET_PORT=31339 python3 exploits/02_crash.py
```

## E3 — pwntools port

Rewrite `04_rip_control.py` using `from pwn import *` if you use pwntools.
Keep the same OFFSET discovery discipline.

## E4 — Compare to Windows

Write a short table: register names, address size, debugger, gadget command
(Windows mona vs Linux ROPgadget/objdump).
