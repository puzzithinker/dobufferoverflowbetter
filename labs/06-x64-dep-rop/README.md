# Lab 06 — x64 DEP-on ret2win

x64 companion to [lab 05](../05-x86-dep-rop/): **DEP/NX on**, ASLR off, call `win()`.

| Item | Value |
|------|--------|
| Binary | `bin/dobetter_rop_x64.exe` |
| Port | **31342** |
| Offset (MinGW appendix) | **~152** — re-measure / extract_lab_meta |
| Pack | `struct.pack('<Q', addr)` |

```bash
make -C labs/06-x64-dep-rop
```

```powershell
..\..\setup\scripts\Disable-TargetMitigations.ps1 -Name dobetter_rop_x64.exe -KeepDEP
```

Tutorial: [TUTORIAL.md](TUTORIAL.md)
