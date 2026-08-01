#!/usr/bin/env bash
# Clone mona3 (and optionally CorelanTraining) into tools/ for packaging or docs.
set -euo pipefail

ALSO_CORELAN=0
if [[ "${1:-}" == "--with-corelan-training" ]]; then
  ALSO_CORELAN=1
fi

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
TOOLS="${ROOT}/tools"
mkdir -p "${TOOLS}"

sync_repo() {
  local url="$1"
  local dest="$2"
  if [[ -d "${dest}/.git" ]]; then
    echo "[*] Updating ${dest}"
    git -C "${dest}" pull --ff-only
  elif [[ -e "${dest}" ]]; then
    echo "[!] Path exists but is not a git repo: ${dest}" >&2
    exit 1
  else
    echo "[*] Cloning ${url} -> ${dest}"
    git clone --depth 1 "${url}" "${dest}"
  fi
}

command -v git >/dev/null || { echo "git is required" >&2; exit 1; }

sync_repo "https://github.com/corelan/mona3.git" "${TOOLS}/mona3"

if [[ "${ALSO_CORELAN}" -eq 1 ]]; then
  sync_repo "https://github.com/corelan/CorelanTraining.git" "${TOOLS}/CorelanTraining"
fi

for f in mona.py windbglib.py; do
  path="${TOOLS}/mona3/${f}"
  if [[ ! -f "${path}" ]]; then
    echo "[!] Missing ${path}" >&2
    exit 1
  fi
  if head -n1 "${path}" | grep -qiE '<!DOCTYPE|<html'; then
    echo "[!] ${path} looks like HTML, not Python" >&2
    exit 1
  fi
done

echo "[+] mona3 mirror ready at ${TOOLS}/mona3"
echo "    On Windows, run: .\\setup\\scripts\\Install-Mona3.ps1"
