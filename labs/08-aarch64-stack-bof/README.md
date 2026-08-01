# Lab 08 — AArch64 stack buffer overflow

Same pedagogical bug on **AArch64 Linux**: length-based stack smash, mitigations
off (`-z execstack -no-pie -fno-stack-protector`).

| Item | Value |
|------|--------|
| Binary | `bin/dobetter_aarch64` |
| TCP | `--serve` **31344** |
| Gadget | `gadget_br_x0` (`br x0`) when built for aarch64 |

## Build

```bash
# On x86_64 host:
sudo apt install gcc-aarch64-linux-gnu qemu-user
make -C labs/08-aarch64-stack-bof

# Native on Raspberry Pi / ARM64 cloud:
make -C labs/08-aarch64-stack-bof
```

## Run

```bash
# native aarch64
./bin/dobetter_aarch64

# or emulate
qemu-aarch64 -L /usr/aarch64-linux-gnu ./bin/dobetter_aarch64
```

## Learning notes

- Return address is still on the stack; AArch64 uses `x30`/LR conventions on
  calls, but a classic stack smash of a local buffer + saved frame can still
  hijack control depending on frame layout — **measure under gdb-multiarch**.
- Registers differ (`x0`–`x30`, `sp`); shellcode and calling convention differ.
- This lab is a **bridge**, not a full AArch64 exploit course.

Tutorial: [TUTORIAL.md](TUTORIAL.md)
