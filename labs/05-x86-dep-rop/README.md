# Lab 05 — DEP on, ROP intro (x86)

Same stack overflow as lab 01, but **DEP/NX is enabled**. Stack shellcode will
fault with an execute violation until you reuse **existing code** (ROP).

| Item | Value |
|------|--------|
| Binary | `bin/dobetter_rop_x86.exe` |
| Port | **31341** |
| ASLR | Off |
| DEP | **On** (`NX_COMPAT`) |
| Part A | **ret2win** → call `win()` |
| Part B | mona ROP → `VirtualProtect` → stack shellcode |

## Prerequisites

- Lab 01 complete (offset + EIP control muscle memory)
- Comfortable reading a few gadgets under WinDBG

## Build

```bash
make -C labs/05-x86-dep-rop
# PE DllCharacteristics should include NX_COMPAT (0x100)
```

## Tutorial

→ [TUTORIAL.md](TUTORIAL.md)

## Mitigations on the guest

ASLR is off in the PE flags; still relax Force ASLR if Windows rewrites the image:

```powershell
# Keep DEP on — only relax ASLR-related force policies
..\..\setup\scripts\Disable-TargetMitigations.ps1 -Name dobetter_rop_x86.exe -KeepDEP
```
