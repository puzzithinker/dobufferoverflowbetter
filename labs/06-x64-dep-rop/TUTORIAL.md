# Tutorial — dobetter_rop_x64

Same story as lab 05 on **x64**: stack shellcode dies under NX; ret2win works.

## Steps

```bash
export TARGET_PORT=31342
python3 exploits/02_crash.py
python3 exploits/03_pattern.py
# mona / cyclic → OFFSET (appendix ~152 for current MinGW; re-measure)
OFFSET=152 python3 exploits/04_rip_control.py
# nm / startup hint for win
OFFSET=152 WIN=0x000000014000.... python3 exploits/06_ret2win.py
```

Payloads must be **≥ 64** bytes (short names take the safe snprintf path).

## Notes

- 8-byte return addresses (`<Q`)
- Shadow space / home space matter for full API ROP later; ret2win ignores that
- After ret2win, try lab 05 Part B concepts on x64 with mona (advanced)

Appendix: [../shared/mingw-offset-appendix.md](../shared/mingw-offset-appendix.md)
