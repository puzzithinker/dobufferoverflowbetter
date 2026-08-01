# Solutions notes (x64)

Same policy as lab 01: **measure offsets and gadgets on your build**.

## Procedure

1. `02_crash.py` → confirm fault in x64 WinDBG
2. Pattern → `!mona findmsp` (watch **RIP**, 8-byte overwrite)
3. `OFFSET=... 04_rip_control.py` → RIP `4242424242424242`
4. Bad chars include `\x00`, `\x0a` at minimum
5. `!mona jmp -r rsp` → `JMP_RSP`
6. INT3 then `windows/x64/exec` shellcode

## Null bytes in addresses

If a gadget address contains embedded nulls that break `%s` delivery, verify
with `dqs rsp` how many bytes of the return address landed. Prefer gadgets
whose little-endian representation stays intact for the bytes you need, or
continue in advanced modules (partial overwrites / different primitives).
