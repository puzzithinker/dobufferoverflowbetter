# Roadmap

Living plan for **do buffer overflow better**. Check boxes as work lands; keep
**Future development** current so the next contributor knows what is intentional
backlog vs out of scope.

---

## Done

### Curriculum and platform

- [x] Curriculum modules 0–9 + resources index  
- [x] mona3 mirror/install/verify scripts + Win11 lab notes  
- [x] `BUILDING.md` (MinGW/MSVC/MSYS2; `MINGW_CC` not host `CC`)  
- [x] CI workflow: `.github/workflows/build.yml`  
- [x] SVG diagrams + WinDBG session map + mock “screenshots”  
- [x] Module 08 SEH, module 09 DEP/ROP  

### Labs

- [x] Lab 01 x86 stack BOF (WinDBG/mona3/Python 3)  
- [x] Lab 02 x64 stack BOF  
- [x] Lab 03 Linux companion (mitigations off) + optional NX build  
- [x] Lab 04 x86 SEH overwrite + egghunter tutorial  
- [x] Lab 05 x86 DEP-on ROP (ret2win + VP skeleton + pinned helpers)  
- [x] Lab 06 x64 DEP-on ret2win  
- [x] Lab 07 Linux NX ret2win (host-tested OFFSET)  
- [x] Lab 08 AArch64 companion (cross-compile in CI)  
- [x] Vulnserver practice skeletons  

### Tooling and reliability

- [x] `scripts/extract_lab_meta.py` + `make meta`  
- [x] MinGW offset appendix (`labs/shared/mingw-offset-appendix.md`)  
- [x] VP stack layout helper (`10_vp_layout_notes.py`) for pinned build  
- [x] Reliable vuln path: short names safe (`name_len < 64`), long names `memcpy` smash  
- [x] Extra exercises per major lab  

### Critical fix (historical note)

Early drafts used a multi-step `Hello+name` write with **live stack indices**.
That could corrupt the copy loop before the saved return address. All SRP labs
now use the short/safe + long/`memcpy` pattern above.

---

## Future development (optional backlog)

These items are **explicitly deferred**. They are good next PRs; nothing here
blocks the current learning path (labs 01→08 + curriculum).

### Documentation and media

| Item | Why it matters | Suggested approach |
|------|----------------|--------------------|
| Real WinDBG / WinDBGX **PNG** screenshots | Replace SVG mocks in `docs/images/` with authentic UI | Capture on a disposable lab VM; strip hostnames; drop PNGs next to mocks and link from `docs/windbg-session-map.md` |
| Expanded per-lab “gotchas” pages | Students hit OS-specific Exploit Protection quirks | One short `TROUBLESHOOTING.md` per Windows lab from real VM sessions |

### Windows exploit depth

| Item | Why it matters | Suggested approach |
|------|----------------|--------------------|
| End-to-end **VirtualProtect** ROP chain on live Win11 | Part B of lab 05 is layout-only without a real `SHELL_ADDR` / ESP | On a snapshot VM: crash under WinDBG, record ESP math, complete `10_vp_layout_notes.py` with `BUILD_ROP=1`, document addresses for a **pinned** MinGW build in the appendix |
| Full **x64 VirtualProtect** (or equivalent) ROP lab | Lab 06 stops at ret2win | New lab or lab 06 Part B: x64 calling convention, shadow space, mona rop, DEP still on |
| SafeSEH / SEHOP teaching binary | Lab 04 is classic SEH with mitigations off | Optional “lab 04b” with SafeSEH-aware module and mona `seh` filtering notes |
| Egghunter-as-primary mini-lab | Egghunter tutorial exists; not a graded path | Optional exercise set with tight post-SEH window and mona `egg` only |

### Automation and CI

| Item | Why it matters | Suggested approach |
|------|----------------|--------------------|
| **qemu-user** CI smoke for AArch64 | Lab 08 builds in CI but does not run | `qemu-aarch64 -L … ./bin/dobetter_aarch64` with a long line; assert non-zero exit or known crash |
| Wine-based PE smoke (optional) | Catch PE load failures without Windows | Only if maintainable; not a substitute for WinDBG labs |
| Publish `make meta` artifact in CI | Offset appendix always matches last green build | Upload `mingw-offset-appendix.generated.md` as a workflow artifact |

### New architectures and tracks

| Item | Why it matters | Suggested approach |
|------|----------------|--------------------|
| Deeper AArch64 path (PC control → shellcode) | Lab 08 is a bridge only | gdb-multiarch tutorials, calling convention, one ret2win-style `win()` on aarch64 |
| ARM32 (AArch32) companion | Completeness for embedded/CTF | Mirror lab 08 shape if demand exists |
| Kernel / driver overflow intro | Beyond user-mode curriculum | Link-out only unless a dedicated maintainer owns it |

### Pedagogy polish

| Item | Why it matters | Suggested approach |
|------|----------------|--------------------|
| Pre-measured offset table for **pinned** MinGW versions in CI | Faster onboarding | Regenerate appendix in CI; still teach mona measurement as mandatory skill |
| Translated curriculum (optional languages) | Accessibility | Keep English canonical; accept translated forks with NOTICE credit |
| Video walkthrough index | Complements Corelan 2026 video | Link external videos only; do not vendor large media in git |

---

## Priority suggestion for contributors

If picking one item next, prefer this order:

1. **Live Win11 validation** of lab 05 VirtualProtect layout (highest teaching payoff).  
2. **Real WinDBG screenshots** (low code risk, high polish).  
3. **x64 DEP/ROP Part B** (extends lab 06).  
4. **qemu-aarch64 CI smoke** (cheap automation).  

---

## Out of scope

- Weaponized exploits against real software or third-party products  
- Vendoring **mona3** (or other large third-party trees) in git — use setup mirror scripts  
- Turning this repo into a full OffSec/Corelan replacement (we link out for paid depth)  
- Guaranteeing one universal OFFSET across MSVC, MinGW, and OS updates without re-measurement  

---

## How to update this file

When you finish a backlog item: move it to **Done** with a short note (lab id or PR).  
When you invent new work: add a row under **Future development**, not a silent TODO in random markdown.
