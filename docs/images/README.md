# Diagrams (and future screenshots)

SVG diagrams render on GitHub and most Markdown viewers.

| File | Topic |
|------|--------|
| [stack-srp-overflow.svg](stack-srp-overflow.svg) | Classic stack SRP overwrite |
| [seh-nseh-layout.svg](seh-nseh-layout.svg) | SEH exploit layout |
| [dep-rop.svg](dep-rop.svg) | DEP off vs on |
| [lab-path.svg](lab-path.svg) | First-party lab path |
| [windbg-mona-mock.svg](windbg-mona-mock.svg) | Mock mona session (placeholder for PNG) |
| [windbg-exchain-mock.svg](windbg-exchain-mock.svg) | Mock `!exchain` (placeholder for PNG) |

## Real WinDBG screenshots (wanted)

Drop PNGs here and link from [../windbg-session-map.md](../windbg-session-map.md). Prefer replacing the `*-mock.svg` files:

- `windbg-mona-load.png` — `!load pykd` / `!mona`
- `windbg-exchain.png` — `!exchain` after SEH crash
- `windbgx-startup.png` — WinDBGX startup settings

Keep personal hostnames out of captures when possible.
