# Shared Python 3 notes for exploits

## Bytes, not str

```python
# good
buf = b"A" * 100
buf += struct.pack("<I", 0x080414C3)   # x86
buf += struct.pack("<Q", 0x0000000140001234)  # x64

# send
sock.sendall(buf + b"\n")
```

## Cyclic patterns without mona (optional)

```python
import string

def cyclic(length: int) -> bytes:
    """De Bruijn-like pattern of 4-byte alphabetic sequences."""
    out = bytearray()
    alphabet = string.ascii_uppercase.encode()
    for a in alphabet:
        for b in alphabet:
            for c in alphabet:
                for d in alphabet:
                    out.extend(bytes([a, b, c, d]))
                    if len(out) >= length:
                        return bytes(out[:length])
    raise ValueError("pattern too long")

def cyclic_find(pattern: bytes | int, length: int = 5000) -> int:
    if isinstance(pattern, int):
        pattern = struct.pack("<I", pattern & 0xFFFFFFFF)
    hay = cyclic(length)
    idx = hay.find(pattern)
    if idx < 0:
        raise ValueError("not found")
    return idx
```

Prefer **mona** in the debugger for consistency with the Corelan workflow:

```text
!mona pattern_create 600
!mona pattern_offset 0x41326341
!mona findmsp
```

## Line-oriented protocol

Both labs treat **newline (`\n`)** as end-of-message. Your payload must not
contain `\n` until the very end (and often must avoid `\x00` inside the buffer
logic). Trailing `\n` is required to trigger `do_response`.
