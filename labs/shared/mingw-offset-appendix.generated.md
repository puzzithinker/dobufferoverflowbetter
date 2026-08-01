# Generated lab metadata
Auto-extracted by `scripts/extract_lab_meta.py`. Re-run after rebuild.
Still verify offsets live with mona/cyclic on Windows.

## 01-x86-stack-bof

Binary: `labs/01-x86-stack-bof/bin/dobetter_x86.exe`
DEP/NX: DllCharacteristics=0x400 NX=off
Frame: response at `ebp-0x90`, buf→RET=148, **OFFSET=148**
Symbols:
- `VirtualProtect@16` @ `0x00000000`
- `do_response` @ `0x00401b1f`
- `gadget_jmp_esp` @ `0x00401550`
- IAT: `0040e1b0 I __imp__VirtualProtect@16`

## 02-x64-stack-bof

Binary: `labs/02-x64-stack-bof/bin/dobetter_x64.exe`
DEP/NX: DllCharacteristics=0x0 NX=off
Frame: response at `rbp-0x90`, buf→RET=152, **OFFSET=152**
Symbols:
- `do_response` @ `0x0000000140001bc5`
- `gadget_jmp_rsp` @ `0x000000014000ab20`
- IAT: `000000014000f2b8 I __imp_VirtualProtect`

## 04-x86-seh-bof

Binary: `labs/04-x86-seh-bof/bin/dobetter_seh_x86.exe`
DEP/NX: DllCharacteristics=0x0 NX=off
SEH frame stack alloc 0x228 (expected packed 512+8 region)
nSEH offset **512**, SEH **516** (from source packing)
Symbols:
- `VirtualProtect@16` @ `0x00000000`
- `gadget_pop_pop_ret` @ `0x00401528`
- `trigger_seh_overflow` @ `0x00401a95`
- IAT: `0040e1b0 I __imp__VirtualProtect@16`

## 05-x86-dep-rop

Binary: `labs/05-x86-dep-rop/bin/dobetter_rop_x86.exe`
DEP/NX: NX_COMPAT present (DEP on)
Frame: response at `ebp-0x90`, buf→RET=148, **OFFSET=148**
Symbols:
- `VirtualProtect@16` @ `0x004085e8`
- `gadget_jmp_esp` @ `0x0040159d`
- `gadget_pop_eax_ret` @ `0x00401594`
- `gadget_push_esp_ret` @ `0x0040159f`
- `gadget_ret` @ `0x0040159c`
- `win` @ `0x00401550`
- IAT: `0040e1b8 I __imp__VirtualProtect@16`

## 06-x64-dep-rop

Binary: `labs/06-x64-dep-rop/bin/dobetter_rop_x64.exe`
DEP/NX: NX_COMPAT present (DEP on)
Frame: response at `rbp-0x90`, buf→RET=152, **OFFSET=152**
Symbols:
- `gadget_jmp_rsp` @ `0x000000014000a9c0`
- `gadget_ret` @ `0x000000014000a9d0`
- `win` @ `0x000000014000151e`
- IAT: `000000014000e2c8 I __imp_VirtualProtect`

## 03-linux-stack-bof

Binary: `labs/03-linux-stack-bof/bin/dobetter_linux`
/home/simon/code/dobufferoverflowbetter/labs/03-linux-stack-bof/bin/dobetter_linux: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=0bcaaabb81fbd44b81b09b8461589a2f9cecbce8, for GNU/Linux 3.2.0, with debug_info, not stripped
GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
0x0000000000000000 0x0000000000000000  RWE    0x10

## 03-linux NX

Binary: `labs/03-linux-stack-bof/bin/dobetter_linux_nx`
**missing**

## 07-linux-nx-ret2win

Binary: `labs/07-linux-nx-ret2win/bin/dobetter_linux_ret2win`
/home/simon/code/dobufferoverflowbetter/labs/07-linux-nx-ret2win/bin/dobetter_linux_ret2win: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=4b4a89f37df6003862f4e7c8eff6763083cf7dfd, for GNU/Linux 3.2.0, with debug_info, not stripped
GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000

## 08-aarch64

Binary: `labs/08-aarch64-stack-bof/bin/dobetter_aarch64`
**missing**

