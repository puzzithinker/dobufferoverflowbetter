# Improvement report for do buffer overflow better

This is an improvement report for this repository. It is a review of the
tree as it stands, not a replacement for [ROADMAP.md](../ROADMAP.md) and not
a change to any lab, tutorial, or workflow.

The audience is a maintainer deciding the next documentation and automation
PRs. Suggestions stay at pedagogy, correctness of what the docs claim, repo
structure, and CI. They do not add payloads, gadget listings, or procedures.

Reviewed: [README.md](../README.md), [ROADMAP.md](../ROADMAP.md),
[CONTRIBUTING.md](../CONTRIBUTING.md), [BUILDING.md](../BUILDING.md), the root
[Makefile](../Makefile), [.github/workflows/build.yml](../.github/workflows/build.yml),
[curriculum/](../curriculum/), [labs/](../labs/) (sources, tutorials, exploit
scripts, Makefiles; not `bin/` as source of truth), [labs/shared/](../labs/shared/),
[scripts/extract_lab_meta.py](../scripts/extract_lab_meta.py), [setup/](../setup/),
[docs/](../docs/), and [third_party/NOTICE.md](../third_party/NOTICE.md).
`tools/mona3` is a local mirror and was not treated as first-party code.

Four read-only passes covered the tree: curriculum and top-level docs, labs
01–04, labs 05–08 plus shared notes, and build/CI/setup. Claims below were
checked against the files they name.

## How to use the priorities

P0 items are false or contradictory statements a learner will hit on the
default path. P1 items are structural: the index, the build docs, and CI do
not describe the repo that is already checked in. P2 items are polish.

Each item states what is in the tree, why it matters, and a direction. Rows
that already appear under **Future development** in `ROADMAP.md` are marked
**existing backlog**. Do not reopen those as if they had been forgotten.

## Existing backlog (do not re-file as new work)

These are the **Future development** rows. The note is only what the tree
already shows, so a later PR can move the row to **Done** or leave it.

| Roadmap row | What is already in the tree |
|-------------|------------------------------|
| Real WinDBG PNG screenshots | SVG mocks and [docs/windbg-session-map.md](windbg-session-map.md) only. [docs/images/README.md](images/README.md) still asks for the PNGs. **Existing backlog.** |
| Per-lab `TROUBLESHOOTING.md` | Not present. One shared [setup/win11-lab-notes.md](../setup/win11-lab-notes.md) exists, and one sentence in it is wrong (P0 below). The new pages are **existing backlog**; the false sentence is not. |
| End-to-end VirtualProtect chain on a live Win11 VM | Lab 05 Part A (ret2win) is real. Part B is a skeleton: `exploits/08_vp_skeleton.py` refuses to send until `BUILD_ROP=1`, and the tutorial checkpoint only requires a written outline. **Existing backlog.** |
| Full x64 VirtualProtect lab | Lab 06 stops at ret2win. **Existing backlog.** |
| SafeSEH / SEHOP teaching binary | Not in the tree. Module 08 defines the terms. **Existing backlog.** Do not treat lab 04 as accidentally missing SafeSEH. |
| Egghunter as its own graded lab | [labs/04-x86-seh-bof/TUTORIAL_EGGHUNTER.md](../labs/04-x86-seh-bof/TUTORIAL_EGGHUNTER.md) exists. The lab README links it. Walkthrough section 7 links [exercises/EXTRA.md](../labs/04-x86-seh-bof/exercises/EXTRA.md) E3 and does not link `TUTORIAL_EGGHUNTER.md`. E3 does not link it either. The roadmap says it is not a graded path. **Existing backlog.** |
| qemu-user CI smoke for AArch64 | CI cross-builds lab 08 and checks `file`. It does not run qemu. **Existing backlog.** The makefile hint also omits `-L` (P2). |
| Wine PE smoke | Not in CI. **Existing backlog.** |
| Publish `make meta` as a CI artifact | CI runs `make meta` and does not upload or diff the file. **Existing backlog.** The committed generated file is also stale (P1). |
| Deeper AArch64 path | Lab 08 is a bridge: source, Makefile, short tutorial, no `exploits/`, no `win`. **Existing backlog.** The curriculum never links the lab (P1). |
| ARM32 companion, kernel intro, translations, video index | Not in the tree, matching the roadmap. **Existing backlog** or explicitly optional. |
| Pinned MinGW offset table regenerated in CI | The hand appendix and `make meta` exist and are marked **Done**. A CI-owned pin is still the backlog row. |

`ROADMAP.md` **Out of scope** stays out of scope: no weaponized exploits, no
vendored mona3, no OffSec/Corelan replacement, no promise of one offset for
every compiler.

`ROADMAP.md` **Done** is largely accurate: labs 01–08 exist, CI builds them,
and extra exercises exist for labs 01–05. The problems below are drift inside
that done work, not missing labs.

## P0 — Learners are told the wrong model

### 1. Lab 01 still teaches the old `sprintf` overflow

**Observed.** [labs/01-x86-stack-bof/src/dobetter_x86.c](../labs/01-x86-stack-bof/src/dobetter_x86.c)
copies with `snprintf` when `name_len < 64` and with `memcpy` when
`name_len >= 64`. The file header says the copy is length-based so a name may
contain a null byte, and that newline still ends the line. The same split is
in labs 02, 03, 05, 06, 07, and 08, and in
[labs/shared/mingw-offset-appendix.md](../labs/shared/mingw-offset-appendix.md)
(“Vulnerable path”). `ROADMAP.md` records this as the fix for the old
`Hello+name` loop.

[labs/01-x86-stack-bof/TUTORIAL.md](../labs/01-x86-stack-bof/TUTORIAL.md)
sections 1 and 2 still say `sprintf` walks past `response`, and the source
review shows `sprintf(response, "Hello %s!!!\n", name);`. That call is not in
the C file. Later in the same tutorial, the offset note and the bad-character
section describe the length-based copy and say a null byte is allowed.
[labs/01-x86-stack-bof/solutions/README.md](../labs/01-x86-stack-bof/solutions/README.md)
repeats the `sprintf` layout and lists a null byte as a bad character.
[labs/02-x64-stack-bof/solutions/README.md](../labs/02-x64-stack-bof/solutions/README.md)
does the same (null required, “`%s` delivery”), while
[labs/02-x64-stack-bof/TUTORIAL.md](../labs/02-x64-stack-bof/TUTORIAL.md) says
nulls are allowed.
[labs/shared/python_helpers.md](../labs/shared/python_helpers.md) says “Both
labs” and that payloads often must avoid a null byte.
[labs/shared/mona-gadgets.md](../labs/shared/mona-gadgets.md) matches the C
sources: interior nulls are allowed, newline is not.

**Why it matters.** The first tutorial’s “open the source” step does not
match the source. A student who trusts the solution key will look for a
`Hello ` prefix in the overflow and will treat nulls as bad on a lab that
was changed so addresses can contain them.

**Direction.** Make sections 1–2 and both solution notes describe the short
`snprintf` path and the long `memcpy` path. Point bad characters at newline
(and whatever mona compare actually finds), and say nulls are legal in the
copy. Rewrite or delete `python_helpers.md` so it matches `mona-gadgets.md`.

### 2. Worked numbers disagree with the offset appendix

**Observed.** The hand appendix and
[labs/shared/mingw-offset-appendix.generated.md](../labs/shared/mingw-offset-appendix.generated.md)
agree on these example offsets for the current MinGW frames: labs 01 and 05
are 148, labs 02 and 06 are 152, lab 04 is nSEH 512 / SEH 516.

These hints do not:

- [labs/01-x86-stack-bof/exploits/04_eip_control.py](../labs/01-x86-stack-bof/exploits/04_eip_control.py),
  `05_badchars.py`, and `06_shellcode.py` print `OFFSET=146`.
- [labs/06-x64-dep-rop/exploits/04_rip_control.py](../labs/06-x64-dep-rop/exploits/04_rip_control.py)
  prints `OFFSET=178` and calls it the appendix default.
  [labs/06-x64-dep-rop/README.md](../labs/06-x64-dep-rop/README.md),
  [TUTORIAL.md](../labs/06-x64-dep-rop/TUTORIAL.md), and
  `exploits/06_ret2win.py` (default `"152"`) say 152.
- [labs/05-x86-dep-rop/exploits/09_ret2win_pinned_mingw13.py](../labs/05-x86-dep-rop/exploits/09_ret2win_pinned_mingw13.py)
  sets `DEFAULT_OFFSET = 148`, which matches the appendix, but the docstring
  tells the reader to override with `OFFSET=154` and a different `WIN`.
  [labs/05-x86-dep-rop/TUTORIAL.md](../labs/05-x86-dep-rop/TUTORIAL.md) Part A
  shows that other `WIN` sample, not the constant in the script. The same
  tutorial says the hand appendix lists `VP_STUB` / `JMP_ESP` example
  addresses. The hand appendix does not. `exploits/10_vp_layout_notes.py`
  carries its own baked defaults.

The hand appendix’s lab 07 example also hardcodes a `WIN` literal. `nm` on
`labs/07-linux-nx-ret2win/bin/dobetter_linux_ret2win` in this workspace does
not match that literal. The generated lab 07 section has no symbol lines.

**Why it matters.** The scripts that run when a variable is missing are the
ones students copy. Two of them advertise an offset the appendix contradicts,
and the “pinned” lab 05 helper documents a different pin than the one it
runs. `ROADMAP.md` already says one universal offset is out of scope. These
are not that promise. They are three local documents that disagree about the
same build.

**Direction.** Delete baked addresses from exploit scripts. Where a hint must
remain, print “measure; hand appendix example is 148 / 152” and keep a single
source. Generate any pinned helper from `make meta` instead of editing it by
hand. Drop the literal `WIN` from the hand appendix.

### 3. The Win11 notes say every lab is built with DEP off

**Observed.** [setup/win11-lab-notes.md](../setup/win11-lab-notes.md) section 4
says lab binaries are built with “no GS cookie, no ASLR, no DEP” and shows
`Disable-TargetMitigations.ps1` only for `dobetter_x86.exe` and
`dobetter_x64.exe`, plus a manual command that disables DEP. The firewall
example lists ports 31337, 31338, 31340, 31341, and 31342.

[labs/05-x86-dep-rop/Makefile](../labs/05-x86-dep-rop/Makefile) and
[labs/06-x64-dep-rop/Makefile](../labs/06-x64-dep-rop/Makefile) pass
`-Wl,--nxcompat`. The matching `Makefile.nmake` files pass `/NXCOMPAT`.
Both lab READMEs tell the reader to pass `-KeepDEP`. The script supports
that switch and says it is for lab 05. Nothing in the Win11 notes uses it,
and the notes never name `dobetter_seh_x86.exe`, `dobetter_rop_x86.exe`, or
`dobetter_rop_x64.exe`. [curriculum/01-platform.md](../curriculum/01-platform.md)
sends every student to this file. Its own example only relaxes
`dobetter_x86.exe`.

**Why it matters.** This is the page people follow before any lab. Applying
it to the DEP labs turns off the mitigation those labs exist to teach.
Per-lab troubleshooting pages are **existing backlog**. This sentence is a
correction to a page that already exists.

**Direction.** Split the note: labs 01, 02, and 04 link DEP off; labs 05 and
06 link DEP on and must use `-KeepDEP`. Name each image. Leave the “write a
TROUBLESHOOTING.md per lab” roadmap row alone.

### 4. In-module “Next” links skip SEH and DEP

**Observed.** [curriculum/README.md](../curriculum/README.md) teaches files in
this order: 00, 01, 02, 03, 04, 05, 06, then **08**, **09**, then **07**.
The root [README.md](../README.md) suggested path matches that (lab 01, then
Vulnserver, x64, optional Linux, SEH, DEP/ROP, then module 07).

Footers do not. [curriculum/05-x64-lab.md](../curriculum/05-x64-lab.md) offers
`06-linux.md` or `07-next-level.md`. [curriculum/06-linux.md](../curriculum/06-linux.md)
goes only to `07-next-level.md`. [curriculum/08-seh-and-beyond.md](../curriculum/08-seh-and-beyond.md)
has no Next footer. [curriculum/00-overview.md](../curriculum/00-overview.md)
weekly rhythm ends on day 7 at lab 02. Learning outcomes in the curriculum
index are “after modules 0–5”, which in that table is lab 02, and they already
include DEP and ASLR.

[docs/images/lab-path.svg](images/lab-path.svg) is the picture under the root
path. Its boxes stop at labs 01–05 plus Vulnserver. The caption says
“01 → 04 SEH / egghunter → 05 DEP ret2win → courses” and “CI builds all of
the above via make all linux seh rop”. Root `make` / `make all` builds only
labs 01 and 02 ([Makefile](../Makefile)). CI also builds `rop64`,
`linux-ret2win`, `Makefile.nx`, and `aarch64`
([.github/workflows/build.yml](../.github/workflows/build.yml)).

[labs/05-x86-dep-rop/TUTORIAL.md](../labs/05-x86-dep-rop/TUTORIAL.md) then
sends the reader to the egghunter tutorial and to module 07, which is the
reverse of the index (egghunter is before DEP).

**Why it matters.** A student who clicks Next, or who trusts the front-page
diagram, finishes the course before labs 04–08. The index and the root path
already have the right order. The footers and the SVG do not.

**Direction.** Point every module footer at the index order: 05 → 06
(optional) → 08 → 09 → 07. Extend the week-one table, or label it “first
week only”. Redraw `lab-path.svg` so labs 06–08 are optional branches, and
stop saying `make all` builds every lab. Renumbering `07-next-level.md` so
the filename sorts last is worth doing in the same PR, with redirects from
the old name.

### 5. Lab 04’s copy loop can overwrite its own index

**Observed.** [labs/04-x86-seh-bof/src/dobetter_seh_x86.c](../labs/04-x86-seh-bof/src/dobetter_seh_x86.c)
`trigger_seh_overflow` copies with `for (i = 0; i < len; i++)` into
`frame.buf`. Lab 01’s source comment says a single `memcpy` is required so
the copy does not corrupt its own index. On the MinGW PE in this workspace,
`trigger_seh_overflow` keeps the buffer at `ebp-0x218`, the nSEH slot at
`ebp-0x18` (512 bytes from the buffer, matching the appendix), and the loop
index at `ebp-0xc`. That index is 524 bytes from the start of the buffer,
four bytes past the SEH slot.
[labs/04-x86-seh-bof/exploits/05_badchars.py](../labs/04-x86-seh-bof/exploits/05_badchars.py)
sends padding, eight marker bytes, then the test bytes. With the documented
offset 512, those test bytes start at 520 and therefore land on the index.

The same function faults after every copy, including a short name. There is
no `name_len < 64` safe path.
[labs/04-x86-seh-bof/exploits/01_connect.py](../labs/04-x86-seh-bof/exploits/01_connect.py)
still sends `student`, the same happy-path script as labs 01–03. The lab 04
tutorial never mentions that script. It starts at the crash step.

**Why it matters.** A bad-character compare will show a torn buffer even when
the socket delivered the bytes. A learner who runs script 01 first, as lab 01
trained them to, kills the server before a debugger is attached.

**Direction.** Copy with one `memcpy`, or place the index where the overflow
cannot reach it. Add a `make meta` check that the index slot is outside the
copied range. Make script 01 say that every request faults, or skip the fault
until the copy passes the buffer.

### 6. The pattern scripts do not load the file mona writes

**Observed.** Labs 01, 02, and 04 `exploits/03_pattern.py` look for
`C:\mona-work\<lab>\pattern.txt`. If that file is missing they send
`common.cyclic()`, which is 4-byte groups drawn only from `A`–`Z`.
The mona3 mirror’s pattern command writes `pattern.md` (usage string:
“Output will be written to pattern.md”), not `pattern.txt`.
[labs/shared/python_helpers.md](../labs/shared/python_helpers.md) shows the
same `A`–`Z` generator and then tells the reader to use `!mona findmsp`.
[labs/03-linux-stack-bof/TUTORIAL.md](../labs/03-linux-stack-bof/TUTORIAL.md)
says to run `python3 exploits/03_pattern.py | ./bin/dobetter_linux`.
[labs/03-linux-stack-bof/exploits/03_pattern.py](../labs/03-linux-stack-bof/exploits/03_pattern.py)
does not print the pattern. `send_payload` in that lab’s `common.py` already
spawns the binary and returns the program’s reply.

**Why it matters.** Offset measurement is the first real step in labs 01–04.
The documented command either sends a pattern mona will not recognize, loads
a path mona does not create, or, on Linux, pipes the wrong stream into a
second copy of the binary.

**Direction.** Point `MONA_PATTERN` at `pattern.md` and send only the ASCII
pattern line, or generate the same pattern mona uses. Say in one sentence
that `cyclic()` and mona are not interchangeable. Change the lab 03 tutorial
to `MODE=local` under the debugger the learner already attached, instead of
a pipe.

### 7. Bad-character defaults reject the null the labs just allowed

**Observed.** Lab 01 and lab 02 tutorials say the length-based copy allows a
null. Their `05_badchars.py` defaults `EXCLUDE_HEX` to `000a`, so the test
set never contains a null. The shellcode step on those pages excludes null
and newline in the encoder example. Lab 04’s script defaults to excluding
only newline, which matches the protocol, but the tutorial’s mona filter on
the redirect step still drops null. The sample address on that page sits in
the low 16MB of the image, so the packed pointer contains a null. Lab 02’s
tutorial never states the 64-byte safe path that lab 01’s section 6 does,
even though `dobetter_x64.c` uses the same split.
[labs/03-linux-stack-bof/README.md](../labs/03-linux-stack-bof/README.md)
lists “i386 if you build `-m32`”. The Makefile has no `-m32` target, and the
scripts pack 8-byte values only.

**Why it matters.** A learner who follows the filter will discard the
in-module trampoline the binary was built to provide, and the compare script
will not let them check the claim that a null survives. Lab 02 looks like
“the bug disappeared” on a short line. The `-m32` line describes a build that
is not wired up.

**Direction.** Default the compare set to include null. Keep newline as the
protocol bad byte. Tell the encoder step that a null can be legal in the
copy and still awkward inside shellcode text. Add the 64-byte rule to the
lab 02 tutorial. Drop the `-m32` claim or add a real target. Do not publish
a sample trampoline address in the tutorial.

## P1 — The index, the build, and CI describe an older repo

### 5. CONTRIBUTING.md still asks for work that is done

**Observed.** [CONTRIBUTING.md](../CONTRIBUTING.md) lists the highest-value
PRs as screenshots, a “DEP-on / ROP intro lab”, “CI: `make all linux seh`”,
and portability. `ROADMAP.md` **Done** already includes labs 05 and 06 and
`.github/workflows/build.yml`. The roadmap’s own priority list is different:
live Win11 validation of lab 05, real screenshots, x64 DEP/ROP Part B, then
qemu-aarch64 smoke.

**Why it matters.** A new contributor who follows CONTRIBUTING will rebuild
labs and CI that are checked in, and will not see the backlog the roadmap
asks them to pick up.

**Direction.** Replace that bullet list with the four roadmap priority items
and a link to **Future development**. Screenshots stay; they are **existing
backlog**. The DEP lab and the CI workflow should not.

### 6. Labs 07 and 08, and the second Linux NX target, fall off the path

**Observed.**

- [BUILDING.md](../BUILDING.md) quick matrix stops at lab 06. The MSVC
  walkthrough only runs `nmake` for labs 01 and 02. `Makefile.nmake` also
  exists for labs 04, 05, and 06. The root README and `make help` include
  `make linux-ret2win` and `make aarch64`. [labs/README.md](../labs/README.md)
  build block stops at `make rop64`.
- [curriculum/resources.md](../curriculum/resources.md) says this project’s
  labs are “01–06 + vulnserver-practice”.
- No curriculum file links [labs/08-aarch64-stack-bof/](../labs/08-aarch64-stack-bof/).
  [curriculum/06-linux.md](../curriculum/06-linux.md) links lab 03 only.
  Lab 07 appears as a row inside [curriculum/09-dep-rop.md](../curriculum/09-dep-rop.md)
  and nowhere in the root suggested path or the SVG.
- The same module’s “Linux twin” tells the reader to build
  `labs/03-linux-stack-bof` with `Makefile.nx` and to expect a non-executable
  stack. That makefile builds `dobetter_linux.c` to `dobetter_linux_nx` and
  does not export `win`. [labs/05-x86-dep-rop/exercises/EXTRA.md](../labs/05-x86-dep-rop/exercises/EXTRA.md)
  E4 still says to add a `win()` to that rebuild. Lab 07 already is that
  exercise: non-executable stack, `win`, and `exploits/06_ret2win.py`.
- Labs 06, 07, and 08 have no `exercises/EXTRA.md`. Labs 01–05 do.
  `ROADMAP.md` marks “extra exercises per major lab” **Done**. Do not re-open
  that as a missing feature for 01–05. Say plainly that 06–08 were not in
  that pass.
- [labs/vulnserver-practice/README.md](../labs/vulnserver-practice/README.md)
  lists `exploits/notes_template.md`. The file is
  [labs/vulnserver-practice/notes_template.md](../labs/vulnserver-practice/notes_template.md).

**Why it matters.** The root repo map lists eight labs. The build page, the
lab index build block, the resource index, and the Linux module stop earlier.
The one Linux NX exercise that asks the student to invent `win()` points at
the binary that does not have it.

**Direction.** Extend the `BUILDING.md` matrix and the `labs/README.md` build
block to every root `Makefile` target, including `Makefile.nx`, and state
that `make` alone is labs 01 and 02. Show `nmake /f Makefile.nmake` for labs
04–06. In module 6 and `resources.md`, list lab 07 as the NX ret2win and lab
08 as a bridge only (do not write the deeper AArch64 course; that is
**existing backlog**). Point module 9’s shell block and lab 05 E4 at lab 07.
Leave `Makefile.nx` as an optional “NX without `win`” note. Fix the notes
template path.

### 7. `make meta` cannot fail, and the committed output is a partial host run

**Observed.** Root `make meta` writes
`labs/shared/mingw-offset-appendix.generated.md`. CI runs it after building
every lab, including AArch64, and then ignores the file.
`scripts/extract_lab_meta.py` always returns 0. Missing tools become
`# error:` text inside the report.

The committed generated file still says `dobetter_linux_nx` and
`labs/08-aarch64-stack-bof/bin/dobetter_aarch64` are **missing**. In this
workspace those two binaries are absent as well (lab 08 was not built here;
`bin/` is gitignored). The lab 03 section embeds an absolute
`/home/simon/code/dobufferoverflowbetter/...` path and a BuildID. That cannot
be what a clean CI runner would write, and nothing diffs it.

The ELF loop keeps a `readelf -l` line when it contains `GNU_STACK` or `RWE`,
or when the stripped line starts with `RW` and also contains `0x`. Lab 07’s
flag line is `RW` and starts with `0x`, so it is dropped. Lab 03’s `RWE` line
is kept. A reader of the generated file cannot see that lab 07’s stack is
non-executable. `readelf -l` on the lab 07 binary in this workspace shows
`GNU_STACK` with `RW`.

The hand appendix’s regen recipe is `make all linux seh rop rop64` plus lab
07. It never builds lab 08 or `Makefile.nx`. The script also does not record
`nm` symbols or an offset for the ELF labs.

Publishing the generated file as a workflow artifact is **existing backlog**.
The gap here is that the checked-in copy is already a stale, machine-specific
snapshot, and the parser hides the NX bit it was meant to show.

**Direction.** Make the extractor fail when a binary it was asked to describe
is missing. Record the `GNU_STACK` flag token for both `RW` and `RWE`, and
record `nm` names for labs 07 and 08 without treating the addresses as
stable. Either stop committing the generated file and upload the CI copy
(the backlog row), or `git diff --exit-code` it after `make meta` once the
absolute paths and BuildIDs are stripped. Put `make aarch64` and
`Makefile.nx` in the hand regen steps.

### 8. MSVC flags and lab 06 gadgets do not match the shared notes

**Observed.**

- [labs/01-x86-stack-bof/Makefile.nmake](../labs/01-x86-stack-bof/Makefile.nmake)
  links `/NXCOMPAT:NO /SAFESEH:NO`.
  [labs/04-x86-seh-bof/Makefile.nmake](../labs/04-x86-seh-bof/Makefile.nmake)
  links `/NXCOMPAT:NO` and does not pass `/SAFESEH`. The MinGW makefile for
  lab 04 correctly refuses `--no-seh`. `BUILDING.md` says “x86 also
  `/SAFESEH:NO`” in the section that only builds labs 01 and 02, and it says
  `/NXCOMPAT:NO` there too, while labs 05 and 06 nmake use `/NXCOMPAT`.
- [labs/06-x64-dep-rop/src/dobetter_rop_x64.c](../labs/06-x64-dep-rop/src/dobetter_rop_x64.c)
  emits `gadget_ret` and `gadget_jmp_rsp` only under `__GNUC__`. There is no
  MSVC fallback. Lab 02 does have an MSVC `gadgets` section.
  [labs/shared/mona-gadgets.md](../labs/shared/mona-gadgets.md) describes that
  MSVC x64 section as how “this repo” embeds gadgets, and it does not mention
  lab 05’s pop/ret teaching symbols, lab 07’s `gadget_ret`, or lab 08’s
  `gadget_br_x0`.
- CI is `ubuntu-latest` only. No job runs `nmake`.
  [setup/scripts/Verify-MonaSetup.ps1](../setup/scripts/Verify-MonaSetup.ps1)
  checks sources for labs 01 and 02 only.

**Why it matters.** The primary lab machine is Windows. A reader who copies
the `BUILDING.md` MSVC flag sentence onto lab 05 or 06 disables DEP. An MSVC
build of lab 06 does not contain the teaching gadgets the MinGW build prints
at startup, and the shared mona note implies that it does. SafeSEH left at
the toolchain default makes the MSVC lab 04 binary a different lesson from
the MinGW one CI actually builds. A Windows nmake job is new work; documenting
the flags that are already in the makefiles is not.

**Direction.** In `BUILDING.md`, give the link flags per lab: DEP off for
01, 02, and 04; `/SAFESEH:NO` on lab 04 nmake if the lesson is “SEH works,
SafeSEH is off”; DEP on for 05 and 06. In `mona-gadgets.md`, a per-lab symbol
table with no addresses: lab 06 MSVC has `win` and no teaching gadget. Add
those gadgets for MSVC or say they are MinGW-only. A `windows-latest` nmake
job is worth it once the flags are honest. Extending Verify’s source list to
labs 04–06 is a small companion.

### 9. Module 9 presents three DEP labs that are not the same exercise

**Observed.** [curriculum/09-dep-rop.md](../curriculum/09-dep-rop.md) lists
labs 05, 06, and 07 as one table.

- Lab 05 has `exploits/01` through `10`, `exercises/EXTRA.md`, and
  `solutions/README.md`. The C file exports `win` and several teaching
  gadgets, and it imports `VirtualProtect`. Part B does not complete a chain
  (see the backlog row).
- Lab 06 has `01`–`04` and `06_ret2win.py` only. No bad-character script, no
  `exercises/`, no `solutions/`. The source does not reference
  `VirtualProtect`. The tutorial tells the reader to try lab 05 Part B “on
  x64” anyway. Full x64 API ROP is **existing backlog**. The tutorial should
  not assign it early.
- Lab 07 has `exploits/06_ret2win.py` and `common.py` only. `win()` writes
  the `[WIN]` line with `write(STDOUT_FILENO, ...)` and then `_exit`. It does
  not write to the client socket. CI runs the script in default local mode
  and greps stdout ([.github/workflows/build.yml](../.github/workflows/build.yml)
  sets `OFFSET=152`). The README also documents `--serve` on port 31343.
  A TCP run of the only script will not see `[WIN]` on the socket.

**Why it matters.** “Do labs 05 and 06” sounds like the same exercise twice.
Lab 06 cannot host the Part B the tutorial points at. Lab 07’s advertised
TCP mode cannot show the success string the local mode and CI depend on.

**Direction.** In the module table, mark 06 and 07 as ret2win-only. Remove
the “try Part B on x64” line until the x64 backlog lab exists. Either write
`win`’s message to the same fd `do_response` uses, or state that local mode
is the supported check and `--serve` will not carry `[WIN]`. Adding the
missing 06/07 scripts is optional and should stay skeletons.

### 10. The Vulnserver checkpoint asks for more than the pack contains

**Observed.** [curriculum/04-vulnserver.md](../curriculum/04-vulnserver.md)
asks for two commands “exploited cleanly”, with notes for offsets, bad
characters, and gadgets. The pack has `01_help.py` through `04_eip_marker.py`
and no bad-character or shellcode script.
[curriculum/04-vulnserver-tutorial.md](../curriculum/04-vulnserver-tutorial.md)
describes redirect and shellcode steps in prose and does not link
`notes_template.md`. [curriculum/08-seh-and-beyond.md](../curriculum/08-seh-and-beyond.md)
step 3 says to do one SEH-oriented Vulnserver command with this pack. The
pack has no SEH script. The notes template has an “EIP / SEH” field, so the
idea is there. The path and the checkpoint oversell the scripts.

**Why it matters.** Module 4 reads like lab 01’s ladder. The folder stops at
a marker, and module 8 sends the student back to a pack that cannot do the
step.

**Direction.** Say in the practice README and in module 4 that the scripts
stop at EIP control on purpose. Point the notes template at the real path
and link it from the tutorial. Change the module 8 step to “use the notes
template’s SEH field” or drop it until a skeleton exists. Do not add a
finished Vulnserver exploit. Egghunter-as-primary remains **existing backlog**.

## P2 — Polish that is still worth a small PR

### 11. Diagrams exist and the chapters do not use them

**Observed.** [docs/images/README.md](images/README.md) documents
`stack-srp-overflow.svg`, `seh-nseh-layout.svg`, and `dep-rop.svg`. The lab
tutorials use some of them. Modules 03, 08, and 09 do not link them.
[docs/windbg-session-map.md](windbg-session-map.md) is the session chapter
linked from [curriculum/02-tools.md](../curriculum/02-tools.md), and it only
shows an x86 lab 01 crash and a JMP ESP transcript. Replacing the SVG mocks
with PNGs is **existing backlog**.

**Direction.** Link each concept SVG from the matching module. Add one line
to the session map: this page is lab 01; SEH and DEP sessions stay in those
lab tutorials until the PNG row is done.

### 12. CI checks that a file exists more than that the mitigation story held

**Observed.** The workflow requires `NX_COMPAT` on labs 05 and 06 only. It
does not require the bit to be absent on labs 01, 02, and 04. The lab 01
gadget check searches the whole disassembly for the JMP ESP opcode, not the
`gadget_jmp_esp` symbol. There is no equivalent check for lab 02 or for
lab 04’s `gadget_pop_pop_ret`. `GNU_STACK` is not asserted (`RWE` for lab 03,
`RW` for `Makefile.nx` and lab 07). Labs 03 and 08 are not executed. The only
process run is lab 07 local ret2win. `python3 -m py_compile` lists labs 01–07
and vulnserver by hand. Lab 08 has no Python. A new `exploits/` directory is
not compiled until the workflow is edited.

[labs/01-x86-stack-bof/Makefile](../labs/01-x86-stack-bof/Makefile) tries
`--no-seh`, discards stderr, and on failure retries without it inside one
shell `if`. The fallback compiler is not under `set -e`, and `echo` runs
after it, so a failed fallback link still exits 0.

qemu-user smoke and Wine smoke are **existing backlog**. Asserting the
flags CI already has the tools to read is not.

**Direction.** Assert `NX_COMPAT` off for 01, 02, and 04 and on for 05 and
06. Tie the gadget check to the symbol. Assert `GNU_STACK` for labs 03, 03-nx,
and 07. On the Ubuntu runner, pipe a short safe name into labs 03 and 07 and
expect the `Hello` reply. Glob `labs/*/exploits/*.py` for `py_compile`. Make
the lab 01 recipe fail when the compiler fails. Keep listener and qemu work
on the roadmap rows that already describe them.

### 13. Eight copies of the same helper, and a page that still says “both labs”

**Observed.** Each lab’s `exploits/common.py` reimplements `cyclic`,
`connect`, and `send_line`. [labs/shared/python_helpers.md](../labs/shared/python_helpers.md)
is not linked from [labs/README.md](../labs/README.md) or the curriculum,
and it still says payloads often must avoid a null byte.
Lab 05 has two ret2win scripts (`06_ret2win.py` and
`09_ret2win_pinned_mingw13.py`) and two VirtualProtect layouts
(`08_vp_skeleton.py` and `10_vp_layout_notes.py`). The duplicates are how
the stale numbers in P0 item 2 survive.

**Why it matters.** A protocol fix has to be repeated eight times. Students
who find the helper page get the pre-`memcpy` rules.

**Direction.** One shared helper module, or delete the page if mona is the
intended pattern tool. Keep a single ret2win script and a single VP skeleton
in lab 05.

### 14. A few names overclaim

**Observed.** The root README lists CFG next to ASLR, DEP, and canaries.
[curriculum/07-next-level.md](../curriculum/07-next-level.md) puts CFG in one
cell with SafeSEH and SEHOP and never says what it is. No lab covers it.
The same module’s capstone 1 says to turn DEP on for lab 01 and build a
VirtualProtect chain. That chain, on lab 05, is **existing backlog**. The
opening “your next skills” list names egghunters and SEH overflows, and the
next section says to finish modules 08 and 09 before this module.
Module 7 also names “Linux ROP Emporium” with no URL, and `resources.md`
does not list it. [curriculum/06-linux.md](../curriculum/06-linux.md) is
otherwise careful about external links.

**Direction.** One sentence and a Microsoft link for CFG, or drop the acronym
until a module defines it. Retarget capstone 1 to “not supported in-tree yet:
lab 05 Part B”, and do not write the chain. Add a URL for ROP Emporium or
remove the name.

## Suggested order

Cheap corrections first, then the roadmap’s own priority list. None of this
requires a new exploit.

1. P0 items 1–3 and 5–7 (overflow model, offset hints, Win11 DEP sentence,
   lab 04’s indexed copy, pattern-file mismatch, null-byte defaults).
2. P0 item 4 and P1 item 6 (one path PR: Next links, SVG, BUILDING matrix,
   lab 07/08 cross-links, lab 05 E4).
3. P1 item 5 (CONTRIBUTING matches the roadmap priority list).
4. P1 item 7 (extractor records `GNU_STACK`, fails when a binary is missing,
   stops embedding host paths). Pair with the **existing backlog** row that
   uploads `make meta` output.
5. P1 items 8 and 9 (per-lab flags, lab 06 gadget honesty, lab 07 success
   channel, module 9 scope labels).
6. Then the roadmap order that is still real work: live Win11 check of lab
   05 Part B, PNG screenshots, x64 Part B, qemu-aarch64 smoke.

## What this report is not asking for

- A finished VirtualProtect chain, an x64 Part B, a SafeSEH teaching
  binary, or an AArch64 shellcode lab. Those rows already exist and should
  stay descriptive until someone does them on a lab VM.
- Vendoring mona3. [third_party/NOTICE.md](../third_party/NOTICE.md) and
  `.gitignore` already match the setup scripts (`git clone --depth 1`, no
  pin). Pinning a commit is a small reproducibility improvement, not a
  license change.
- Rewriting `ROADMAP.md` in place. Update it only when one of the items
  above actually lands, using the process at the bottom of that file.
