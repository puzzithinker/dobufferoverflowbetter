# Tutorial — Egghunters (space-constrained payloads)

Use after [TUTORIAL.md](TUTORIAL.md) (SEH) or whenever the space **after** your
redirect is too small for full shellcode.

## Problem

```text
[ padding ][ nSEH ][ SEH ][ ??? only a few bytes free ??? ]
```

Full `msfvenom` payloads are often 200–400+ bytes. An **egghunter** is a tiny
stub that **searches process memory** for a tagged payload (“egg”) and jumps to
it.

## Egg format

mona default tag is often `w00t` (you can choose another 4-byte tag):

```text
egg marker (tag × 2) + shellcode
Example:  w00t w00t <shellcode bytes>
```

Place this large blob somewhere the hunter can find it:

- earlier in the same overflow (before nSEH), if size allows  
- another protocol field / second request  
- environment / file mapping (advanced)

## Generate hunter

In WinDBG + mona3:

```text
!mona egg -t w00t
```

Output includes a small x86 stub. Put **that stub** in the tight window (e.g.
after SEH short-jump landing zone). Put **egg + shellcode** in the roomy region.

## Layout sketch (SEH lab)

```text
  [ egg + full shellcode .............. ][ pad ][ nSEH jmp ][ SEH P/P/R ][ hunter ]
       ^ found by hunter                                          starts here
```

Or hunter first if your jump lands earlier — draw your actual ESP/EIP path.

## Bad characters

Hunter and egg must respect the same bad-char set as the rest of the exploit.
Regenerate with mona after finalizing `-cpb`.

## Debug checklist

1. Break when hunter starts (short jmp landing).  
2. Single-step or log until it hits the egg.  
3. Confirm `eip` lands on shellcode after the 8-byte egg prefix (hunter usually
   skips the tag).  
4. If hunter loops forever: egg not in searchable memory, access rights, or
   corrupted tag.

## Practice on lab 04

1. Finish normal SEH → INT3.  
2. Shrink the post-SEH window deliberately (fewer NOPs).  
3. Insert mona egghunter; place `w00tw00t` + INT3 sled higher in the buffer.  
4. Document addresses and final payload size.

## Relation to DEP

Egghunters **do not** bypass DEP. They only solve **space**. With DEP on, the
found page still must be executable (or you ROP first — lab 05).

## References

- Corelan egghunter material on https://www.corelan.be/  
- mona: `!mona egg`, `!mona jmp`  
