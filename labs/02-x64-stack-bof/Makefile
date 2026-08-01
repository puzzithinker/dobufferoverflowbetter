# Cross-compile Win64 PE (Linux/macOS MinGW-w64 or MSYS2).
#
# Linux:
#   sudo apt install mingw-w64 make
#   make
#
# MSYS2 (mingw-w64 x86_64 env):
#   pacman -S mingw-w64-x86_64-gcc make
#   make MINGW_CC=x86_64-w64-mingw32-gcc
#   # or: make MINGW_CC=gcc

MINGW_CC ?= x86_64-w64-mingw32-gcc
CFLAGS   ?= -O0 -g -fno-stack-protector -Wall -Wextra -D_WIN32_WINNT=0x0601
LDFLAGS  ?= -lws2_32 -Wl,--disable-dynamicbase -Wl,--disable-nxcompat

SRC    = src/dobetter_x64.c
OUTDIR = bin
TARGET = $(OUTDIR)/dobetter_x64.exe

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
	$(MINGW_CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "Built $@"
	@file $@ 2>/dev/null || true

clean:
	rm -rf $(OUTDIR)
