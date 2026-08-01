@echo off
REM WinDBG Classic x86 launcher with pykd + mona3 alias
REM Copy next to windbg.exe under Windows Kits\10\Debuggers\x86 or edit WINDBG_EXE.
REM Adjust PYTHON_TAG if CorelanPyKDInstall used 3.14 instead of 3.9.

set "MONA_DIR=C:\Tools\mona3"
set "PYTHON_TAG=3.9"
set "WINDBG_EXE=%~dp0windbg.exe"

if not exist "%WINDBG_EXE%" (
  echo windbg.exe not found next to this script: %WINDBG_EXE%
  echo Place w.bat in Debuggers\x86 or set WINDBG_EXE to a full path.
  exit /b 1
)

"%WINDBG_EXE%" -hd -c "!load pykd; as !mona !py -%PYTHON_TAG% %MONA_DIR%\mona.py" %*
