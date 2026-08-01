# mona3 gadget notes (MinGW vs MSVC)

## Finding trampolines

```text
!mona jmp -r esp -cpb "\x00\x0a"     # x86
!mona jmp -r rsp -cpb "\x00\x0a"     # x64
```

Prefer results **inside the lab module** when ASLR is disabled for learning.

## How this repo embeds gadgets

| Build | Form | mona sees |
|-------|------|-----------|
| MinGW x86 | `gadget_jmp_esp` function = `ff e4` | Normal executable `.text` symbol |
| MinGW x64 | `gadget_jmp_rsp` function = `ff e4` | Same |
| MSVC x86 | `__declspec(naked)` `jmp esp` | `.text` |
| MSVC x64 | `unsigned char` bytes in `#pragma section("gadgets", read, execute)` | Executable section; still `ff e4` if linked |

If mona returns nothing:

1. Confirm you are in the correct arch session (x86 vs x64).
2. `!mona modules` — is the lab module listed? ASLR/rebase?
3. Search manually: `s -b image_start image_end ff e4`
4. On Linux ELF lab: `objdump -d` / ROPgadget (no mona).

## Null bytes in addresses

Little-endian packing may embed `0x00`. This repo’s Windows labs use
**length-based** copies so nulls inside the overflow payload are allowed.
**Newline `0x0a` still ends the TCP line** and remains bad.

Always verify with the debugger that the full return address landed:

```text
# x86
dps esp L10
# x64
dqs rsp L10
```

## Example addresses are build-specific

A MinGW PE on one machine may show `JMP ESP` near `0x00401xxx`. Your MSVC
build will differ. **Never hardcode write-up addresses.**
