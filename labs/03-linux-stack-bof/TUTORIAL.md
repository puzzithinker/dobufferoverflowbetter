# Tutorial — dobetter_linux (gdb + Python 3)

Same overflow shape as the Windows labs; different tooling.

## 1. Build and checksec

```bash
make -C labs/03-linux-stack-bof
# optional: checksec --file=bin/dobetter_linux
readelf -l bin/dobetter_linux | grep GNU_STACK   # expect RWE (exec stack)
```

## 2. Crash under gdb

```bash
gdb -q ./bin/dobetter_linux
(gdb) run
# type a long AAAA... line or:
(gdb) run <<< $(python3 -c 'print("A"*300)')
```

On fault: `info registers`, `x/20gx $rsp`, note RIP.

## 3. Offset

```bash
python3 exploits/03_pattern.py | ./bin/dobetter_linux
# or feed under gdb; compute cyclic offset of RIP bytes
python3 exploits/04_rip_control.py   # set OFFSET=
```

## 4. Bad characters (optional but good habit)

```bash
OFFSET=<n> python3 exploits/05_badchars.py
# inspect under gdb; at least \\n (0x0a) is special in line mode
```

## 5. JMP RSP

```bash
objdump -d bin/dobetter_linux | grep -A1 gadget_jmp
# or: ROPgadget --binary bin/dobetter_linux | grep 'jmp rsp'
```

With NX off (`-z execstack`), ret-to-JMP-RSP + stack shellcode works like the Windows DEP-off labs.

## 6. Shellcode sketch

```python
# local only — e.g. execve /bin/sh (generate with msfvenom or pwntools shellcraft)
# Avoid 0x0a in the payload if using line mode.
```

TCP mode: `./bin/dobetter_linux --serve 31339` then point exploits at `TARGET_PORT=31339`.

## 7. Checkpoint

- [ ] Explain canary/PIE/NX status of this build
- [ ] Reliable RIP control
- [ ] Shell or INT3-equivalent success with stack shellcode
