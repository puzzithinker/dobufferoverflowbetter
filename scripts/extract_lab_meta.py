#!/usr/bin/env python3
"""
Extract teaching metadata from built lab binaries (objdump/nm).

Usage (from repo root, after make):
  python3 scripts/extract_lab_meta.py
  python3 scripts/extract_lab_meta.py --write labs/shared/mingw-offset-appendix.generated.md

Does not replace mona measurement on a live target — documents *this* build.
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(cmd: list[str]) -> str:
    try:
        return subprocess.check_output(cmd, text=True, stderr=subprocess.DEVNULL)
    except (subprocess.CalledProcessError, FileNotFoundError) as e:
        return f"# error: {e}\n"


def parse_do_response_x86(objdump: str) -> dict | None:
    """Find lea -0xNN(%ebp) used as response[] base and sub $0xNN,%esp."""
    m = re.search(
        r"<_do_response>:.*?ret",
        objdump,
        re.S,
    )
    if not m:
        # try without underscore
        m = re.search(r"<do_response>:.*?ret", objdump, re.S)
    if not m:
        return None
    body = m.group(0)
    lea = re.search(r"lea\s+-0x([0-9a-f]+)\(%ebp\)", body)
    # first write site often: -0x9c(%ebp,%eax,1)
    idx = re.search(r"-0x([0-9a-f]+)\(%ebp,%e[acdb]x,1\)", body)
    if not lea and not idx:
        return None
    off = int((idx or lea).group(1), 16)
    # saved ret at ebp+4 → distance from buf[0] = off + 4
    # Vulnerable path memcpys name at response[0] (short names use snprintf).
    dist = off + 4
    return {
        "buf_ebp_off": off,
        "buf_to_ret": dist,
        "name_to_ip": dist,
    }


def parse_do_response_x64(objdump: str) -> dict | None:
    m = re.search(r"<do_response>:.*?ret", objdump, re.S)
    if not m:
        return None
    body = m.group(0)
    idx = re.search(r"-0x([0-9a-f]+)\(%rbp,%r[a-z0-9]+,1\)", body)
    if not idx:
        idx = re.search(r"lea\s+-0x([0-9a-f]+)\(%rbp\)", body)
    if not idx:
        return None
    off = int(idx.group(1), 16)
    dist = off + 8  # saved ret at rbp+8
    return {
        "buf_rbp_off": off,
        "buf_to_ret": dist,
        "name_to_ip": dist,  # memcpy name at response[0]
    }


def nm_symbols(nm_bin: str, path: Path, patterns: list[str]) -> dict[str, str]:
    out = run([nm_bin, str(path)])
    found: dict[str, str] = {}
    for line in out.splitlines():
        # 0040151c T _win
        parts = line.split()
        if len(parts) < 3:
            continue
        addr, typ, name = parts[0], parts[1], parts[-1]
        name = name.lstrip("_")
        for p in patterns:
            if name == p or name.endswith(p):
                found[p] = "0x" + addr.lower().lstrip("0") if addr != "0" else "0x0"
                # keep full
                found[p] = "0x" + addr.lower()
    return found


def pe_nx(path: Path, objdump: str) -> str:
    out = run([objdump, "-x", str(path)])
    if "NX_COMPAT" in out:
        return "NX_COMPAT present (DEP on)"
    # DllCharacteristics hex
    m = re.search(r"DllCharacteristics\s+([0-9a-fA-F]+)", out)
    if m:
        val = int(m.group(1), 16)
        nx = bool(val & 0x100)
        return f"DllCharacteristics=0x{val:x} NX={'on' if nx else 'off'}"
    return "unknown"


def section(title: str, lines: list[str]) -> str:
    return f"## {title}\n\n" + "\n".join(lines) + "\n\n"


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--write", type=Path, help="Write markdown report here")
    args = ap.parse_args()

    report: list[str] = [
        "# Generated lab metadata\n",
        "Auto-extracted by `scripts/extract_lab_meta.py`. Re-run after rebuild.\n",
        "Still verify offsets live with mona/cyclic on Windows.\n\n",
    ]

    labs = [
        {
            "name": "01-x86-stack-bof",
            "exe": ROOT / "labs/01-x86-stack-bof/bin/dobetter_x86.exe",
            "od": "i686-w64-mingw32-objdump",
            "nm": "i686-w64-mingw32-nm",
            "arch": "x86",
            "syms": ["gadget_jmp_esp", "do_response"],
        },
        {
            "name": "02-x64-stack-bof",
            "exe": ROOT / "labs/02-x64-stack-bof/bin/dobetter_x64.exe",
            "od": "x86_64-w64-mingw32-objdump",
            "nm": "x86_64-w64-mingw32-nm",
            "arch": "x64",
            "syms": ["gadget_jmp_rsp", "do_response"],
        },
        {
            "name": "04-x86-seh-bof",
            "exe": ROOT / "labs/04-x86-seh-bof/bin/dobetter_seh_x86.exe",
            "od": "i686-w64-mingw32-objdump",
            "nm": "i686-w64-mingw32-nm",
            "arch": "x86-seh",
            "syms": ["gadget_pop_pop_ret", "trigger_seh_overflow"],
        },
        {
            "name": "05-x86-dep-rop",
            "exe": ROOT / "labs/05-x86-dep-rop/bin/dobetter_rop_x86.exe",
            "od": "i686-w64-mingw32-objdump",
            "nm": "i686-w64-mingw32-nm",
            "arch": "x86",
            "syms": [
                "win",
                "gadget_pop_eax_ret",
                "gadget_jmp_esp",
                "gadget_push_esp_ret",
                "gadget_ret",
            ],
        },
        {
            "name": "06-x64-dep-rop",
            "exe": ROOT / "labs/06-x64-dep-rop/bin/dobetter_rop_x64.exe",
            "od": "x86_64-w64-mingw32-objdump",
            "nm": "x86_64-w64-mingw32-nm",
            "arch": "x64",
            "syms": ["win", "gadget_jmp_rsp", "gadget_ret"],
        },
    ]

    for lab in labs:
        exe: Path = lab["exe"]
        lines = [f"Binary: `{exe.relative_to(ROOT)}`"]
        if not exe.is_file():
            lines.append("**missing — build first**")
            report.append(section(lab["name"], lines))
            continue

        lines.append(f"DEP/NX: {pe_nx(exe, lab['od'])}")
        od = run([lab["od"], "-d", str(exe)])
        if lab["arch"] == "x86":
            meta = parse_do_response_x86(od)
            if meta:
                lines.append(
                    f"Frame: response at `ebp-0x{meta['buf_ebp_off']:x}`, "
                    f"buf→RET={meta['buf_to_ret']}, **OFFSET={meta['name_to_ip']}**"
                )
        elif lab["arch"] == "x64":
            meta = parse_do_response_x64(od)
            if meta:
                lines.append(
                    f"Frame: response at `rbp-0x{meta['buf_rbp_off']:x}`, "
                    f"buf→RET={meta['buf_to_ret']}, **OFFSET={meta['name_to_ip']}**"
                )
        elif lab["arch"] == "x86-seh":
            if "sub    $0x228,%esp" in od or "sub    $0x228" in od:
                lines.append("SEH frame stack alloc 0x228 (expected packed 512+8 region)")
            lines.append("nSEH offset **512**, SEH **516** (from source packing)")

        syms = nm_symbols(lab["nm"], exe, lab["syms"] + ["VirtualProtect@16"])
        if syms:
            lines.append("Symbols:")
            for k, v in sorted(syms.items()):
                lines.append(f"- `{k}` @ `{v}`")
        # IAT VirtualProtect via nm
        nm_out = run([lab["nm"], str(exe)])
        for line in nm_out.splitlines():
            if "imp__VirtualProtect" in line or "imp_VirtualProtect" in line:
                lines.append(f"- IAT: `{line.strip()}`")

        report.append(section(lab["name"], lines))

    # Linux
    for name, path in [
        ("03-linux-stack-bof", ROOT / "labs/03-linux-stack-bof/bin/dobetter_linux"),
        ("03-linux NX", ROOT / "labs/03-linux-stack-bof/bin/dobetter_linux_nx"),
        ("07-linux-nx-ret2win", ROOT / "labs/07-linux-nx-ret2win/bin/dobetter_linux_ret2win"),
        ("08-aarch64", ROOT / "labs/08-aarch64-stack-bof/bin/dobetter_aarch64"),
    ]:
        lines = [f"Binary: `{path.relative_to(ROOT) if path.is_relative_to(ROOT) else path}`"]
        if path.is_file():
            lines.append(run(["file", str(path)]).strip())
            readelf = run(["readelf", "-l", str(path)])
            for line in readelf.splitlines():
                if "GNU_STACK" in line or "RWE" in line or line.strip().startswith("RW"):
                    if "GNU_STACK" in line or "0x" in line:
                        lines.append(line.strip())
        else:
            lines.append("**missing**")
        report.append(section(name, lines))

    text = "".join(report)
    if args.write:
        args.write.parent.mkdir(parents=True, exist_ok=True)
        args.write.write_text(text)
        print(f"wrote {args.write}")
    else:
        sys.stdout.write(text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
