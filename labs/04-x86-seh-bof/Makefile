# Win32 SEH lab — MinGW. Keep SEH enabled (no --no-seh).
MINGW_CC ?= i686-w64-mingw32-gcc
CFLAGS   ?= -O0 -g -fno-stack-protector -Wall -Wextra -D_WIN32_WINNT=0x0601
LDFLAGS  ?= -lws2_32 -Wl,--disable-dynamicbase -Wl,--disable-nxcompat

SRC    = src/dobetter_seh_x86.c
OUTDIR = bin
TARGET = $(OUTDIR)/dobetter_seh_x86.exe

.PHONY: all clean check-compiler

all: check-compiler $(TARGET)

check-compiler:
	@command -v $(MINGW_CC) >/dev/null 2>&1 || { echo "need $(MINGW_CC)"; exit 1; }

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(SRC) | $(OUTDIR)
	$(MINGW_CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "Built $@ (SEH enabled — do not use --no-seh)"
	@file $@ 2>/dev/null || true

clean:
	rm -rf $(OUTDIR)
