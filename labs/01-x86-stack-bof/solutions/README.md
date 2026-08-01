# Solutions notes (x86)

We intentionally **do not hardcode** a universal EIP offset or JMP ESP address:

- Compiler (MSVC vs MinGW) changes stack frames.
- Image base / gadget RVA differ per build.

## Procedure answer key

1. Crash with `02_crash.py`
2. Pattern with `03_pattern.py` + `!mona findmsp`
3. Set `OFFSET` for `04_eip_control.py` until EIP is `42424242`
4. Bad chars: at least `\x00` and `\x0a`
5. `!mona jmp -r esp -cpb "\x00\x0a"` → set `JMP_ESP`
6. INT3 shellcode first, then msfvenom `windows/exec`

## Format string layout hint

`sprintf(response, "Hello %s!!!\n", name)` writes:

```text
Hello <name>!!!\n\0
```

Overflow length is dominated by `name`. Saved EBP (4) + saved RET (4) sit after
the 128-byte `response` array, subject to compiler padding — **measure**.
