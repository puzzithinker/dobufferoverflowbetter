@echo off
REM WinDBG Classic x64 launcher with pykd + mona3 alias
REM Copy next to windbg.exe under Windows Kits\10\Debuggers\x64 or edit WINDBG_EXE.

set "MONA_DIR=C:\Tools\mona3"
set "PYTHON_TAG=3.9"
set "WINDBG_EXE=%~dp0windbg.exe"

if not exist "%WINDBG_EXE%" (
  echo windbg.exe not found next to this script: %WINDBG_EXE%
  echo Place w64.bat in Debuggers\x64 or set WINDBG_EXE to a full path.
  exit /b 1
)

REM Prefer explicit 64-bit Python tag when using Classic x64
"%WINDBG_EXE%" -hd -c "!load pykd; as !mona !py -%PYTHON_TAG%-64 %MONA_DIR%\mona.py" %*
