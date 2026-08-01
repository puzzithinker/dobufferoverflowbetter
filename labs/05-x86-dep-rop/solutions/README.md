# Solutions notes — DEP/ROP lab

## Part A

1. Measure OFFSET as in lab 01 (same buffer size → often same ballpark).  
2. `WIN` = `win` symbol (startup hint or mona).  
3. `06_ret2win.py` → server prints `[WIN]`.  

## Part B

No universal VP chain is committed — mona output differs by build. Record:

| Item | Value |
|------|--------|
| VirtualProtect (IAT) | |
| Writable DWORD | |
| Shellcode addr / size | |
| Chain gadgets | |

## PE flag check (host)

```bash
i686-w64-mingw32-objdump -x bin/dobetter_rop_x86.exe | grep DllCharacteristics
# expect NX_COMPAT (0x100 bit)
```
