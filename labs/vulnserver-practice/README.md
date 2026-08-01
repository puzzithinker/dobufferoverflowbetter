# Vulnserver practice pack

Progressive **skeleton** scripts for [stephenbradshaw/vulnserver](https://github.com/stephenbradshaw/vulnserver).
They do **not** ship offsets or full exploits — you fill those after measuring.

## Setup

1. Get `vulnserver.exe` from upstream.  
2. Windows lab VM: disable Exploit Protection for the image; allow TCP **9999**.  
3. Methodology: [../../curriculum/04-vulnserver-tutorial.md](../../curriculum/04-vulnserver-tutorial.md)

## Scripts

| Script | Purpose |
|--------|---------|
| `exploits/01_help.py` | Banner / HELP |
| `exploits/02_fuzz_cmd.py` | Fuzz one command (`VULN_CMD`) |
| `exploits/03_pattern_cmd.py` | Send cyclic pattern after command |
| `exploits/04_eip_marker.py` | Marker at OFFSET |
| `exploits/notes_template.md` | Copy per command |

```bash
export TARGET_IP=192.168.56.10
export VULN_CMD=TRUN
python3 exploits/02_fuzz_cmd.py
```

## Ethics

Only against your lab copy of Vulnserver.
