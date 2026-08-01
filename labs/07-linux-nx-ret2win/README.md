# Lab 07 — Linux NX ret2win

Same overflow as lab 03, but **stack is non-executable**. Call `win()` via
return-oriented control (minimal ROP).

| Item | Value |
|------|--------|
| Binary | `bin/dobetter_linux_ret2win` |
| TCP | `--serve` port **31343** |
| Flags | `-fno-stack-protector -no-pie` (no `-z execstack`) |

```bash
make -C labs/07-linux-nx-ret2win
./bin/dobetter_linux_ret2win
# or: ./bin/dobetter_linux_ret2win --serve 31343
```

Tutorial: [TUTORIAL.md](TUTORIAL.md)
