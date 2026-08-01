# Tutorial — AArch64 stack BOF (intro)

## Tools

```bash
sudo apt install gcc-aarch64-linux-gnu gdb-multiarch qemu-user
```

## Build and crash

```bash
make -C labs/08-aarch64-stack-bof
qemu-aarch64 -L /usr/aarch64-linux-gnu ./bin/dobetter_aarch64
# send a long line of A's
```

Under gdb:

```bash
gdb-multiarch ./bin/dobetter_aarch64
(gdb) set architecture aarch64
# if using qemu user with gdbstub, see qemu -g 1234
```

## Goals

1. Identify the overflow in `do_response` (same C bug as x86/x64 labs).
2. Crash and note faulting address / register state.
3. Find a controlled return / PC path (layout is compiler-specific — measure).
4. Optionally ret to `gadget_br_x0` after loading a register (advanced).

## Mental model

You already know stack smashing from labs 01–03. Here the **ISA and ABI** change;
the vulnerability class does not. Use this lab to practice toolchains and
cross-debug before specialist ARM courses.

## Checkpoint

- [ ] Built aarch64 ELF  
- [ ] Crashed it under qemu or native  
- [ ] Explained one AArch64 vs x86-64 register difference relevant to exploits  
