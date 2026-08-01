# Extra exercises — SEH lab

## E1 — Draw the chain

From a live `!exchain` before and after overflow, sketch FS:[0] → records → end
marker `0xFFFFFFFF`.

## E2 — Break on handler

```text
bp <your PPR>
g
```

Send the exploit and single-step from POP/POP/RET into the short jump.

## E3 — Egghunter lite

Place `w00t` + shellcode high in a larger payload; put a mona-generated egghunter
immediately after SEH. Document space limits you hit.

## E4 — SafeSEH discussion

With SafeSEH on (not this lab), mona filters handlers. Read Corelan SEH parts
and list what would break in your exploit if SafeSEH were enabled on the module.
