# MinGW / host offset appendix

**Primary source of truth after rebuild:**

```bash
make all linux seh rop rop64
make -C labs/07-linux-nx-ret2win
python3 scripts/extract_lab_meta.py --write labs/shared/mingw-offset-appendix.generated.md
cat labs/shared/mingw-offset-appendix.generated.md
```

The numbers below match a **GCC 13-win32 MinGW** build on the doc author’s host after the
`memcpy` vulnerable path change (`name_len >= 64`). **Re-measure** if anything differs.

## Vulnerable path (all SRP labs)

| Path | Behavior |
|------|----------|
| `name_len < 64` | Safe `snprintf` → `Hello …!!!` (connect scripts) |
| `name_len >= 64` | **`memcpy(response, name, name_len)`** into 128-byte stack buffer |

Exploit payloads must be **≥ 64** bytes. `OFFSET` is from **start of name** to saved IP
(name is copied at `response[0]`).

## Typical offsets (example rebuild)

| Lab | Arch | OFFSET | Notes |
|-----|------|--------|--------|
| 01, 05 | x86 PE | **148** | `ebp-0x90` → RET |
| 02, 06 | x64 PE | **152** | `rbp-0x90` → RET |
| 04 SEH | x86 PE | **512** nSEH / **516** SEH | packed frame |
| 07 Linux NX ret2win | x86-64 ELF | **152** (host-tested) | `ret2win` verified |

## Lab 05 symbols (example)

Run `i686-w64-mingw32-nm labs/05-x86-dep-rop/bin/dobetter_rop_x86.exe | grep -E 'win$|gadget_'`.

See `.generated.md` for the latest addresses on your tree.

## Lab 07 (Linux) host-tested ret2win

```bash
make -C labs/07-linux-nx-ret2win
nm bin/dobetter_linux_ret2win | grep ' win$'
OFFSET=152 WIN=0x4013f6 python3 exploits/06_ret2win.py   # WIN from your nm
```

## Always

Trust **mona / cyclic / gdb** over this file after a rebuild or MSVC compile.
