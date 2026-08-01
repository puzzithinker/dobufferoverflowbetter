# Contributing

Thanks for helping make buffer-overflow learning clearer.

## Scope

See [ROADMAP.md](ROADMAP.md) for the current backlog. Highest-value PRs:

- Real WinDBG/mona3 screenshots under `docs/images/`
- DEP-on / ROP intro lab (mitigations partially on)
- CI: `make all linux seh` on Ubuntu
- Portability fixes (MSVC / MinGW / MSYS2)

## Out of scope

- Weaponized exploits against real software
- Bypassing third-party products for abuse
- Committing mirrored `tools/mona3` (use setup scripts)

## Style

- Prefer complete sentences in tutorials
- Python 3 only for new exploit material
- Measure offsets — do not pretend one EIP offset fits all builds
- Keep ethics/lab isolation language intact

## Credit

If you adapt ideas from dostackbufferoverflowgood or Corelan, keep attribution in
[third_party/NOTICE.md](third_party/NOTICE.md) and commit messages.
