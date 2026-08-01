# Cross-compile Win32 PE (Linux/macOS MinGW-w64 or MSYS2).
#
# Linux:
#   sudo apt install mingw-w64 make
#   make
#
# MSYS2 (mingw-w64 i686 env):
#   pacman -S mingw-w64-i686-gcc make
#   make MINGW_CC=i686-w64-mingw32-gcc
#   # or if your triplet differs:
#   make MINGW_CC=gcc
#
# Do NOT rely on the environment CC variable (often set to host "cc").

MINGW_CC ?= i686-w64-mingw32-gcc
CFLAGS   ?= -O0 -g -fno-stack-protector -Wall -Wextra -D_WIN32_WINNT=0x0601
LDFLAGS  ?= -lws2_32 -Wl,--disable-dynamicbase -Wl,--disable-nxcompat

# --no-seh is optional (older toolchains may lack it); try with, fall back without.
LDFLAGS_NOSEH ?= $(LDFLAGS) -Wl,--no-seh

SRC    = src/dobetter_x86.c
OUTDIR = bin
TARGET = $(OUTDIR)/dobetter_x86.exe

.PHONY: all clean check-compiler

all: check-compiler $(TARGET)

check-compiler:
	@command -v $(MINGW_CC) >/dev/null 2>&1 || { \
	  echo "error: $(MINGW_CC) not found on PATH"; \
	  echo "  Linux: sudo apt install mingw-w64"; \
	  echo "  Or: make MINGW_CC=<your-mingw-gcc>"; \
	  exit 1; \
	}

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(SRC) | $(OUTDIR)
	@if $(MINGW_CC) $(CFLAGS) -o $@ $< $(LDFLAGS_NOSEH) 2>/dev/null; then \
	  echo "Built $@ (with --no-seh)"; \
	else \
	  echo "Retry without --no-seh..."; \
	  $(MINGW_CC) $(CFLAGS) -o $@ $< $(LDFLAGS); \
	  echo "Built $@"; \
	fi
	@echo "Remember: disable Exploit Protection on the guest for this EXE."
	@file $@ 2>/dev/null || true

clean:
	rm -rf $(OUTDIR)
