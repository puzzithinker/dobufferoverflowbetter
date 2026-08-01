# Solutions notes (Linux)

Offsets depend on compiler and libc framing — **measure with cyclic + gdb**.

## Procedure

1. `gdb ./bin/dobetter_linux` → run with pattern  
2. `info registers rip` → cyclic find  
3. `objdump -d` / ROPgadget for `jmp rsp`  
4. INT3 or `shellcraft` with NX off  

## Gadget (example build only)

On one GCC build, `gadget_jmp_rsp` was near `0x4013d6` as bare `ff e4`.
Your address will differ with PIE (this lab disables PIE).
