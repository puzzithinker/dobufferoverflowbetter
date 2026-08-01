# DEP ON (NX), ASLR OFF — ROP teaching lab
MINGW_CC ?= i686-w64-mingw32-gcc
CFLAGS   ?= -O0 -g -fno-stack-protector -Wall -Wextra -D_WIN32_WINNT=0x0601
# enable NX, disable dynamic base; keep SEH available
LDFLAGS  ?= -lws2_32 -Wl,--disable-dynamicbase -Wl,--nxcompat

SRC    = src/dobetter_rop_x86.c
OUTDIR = bin
TARGET = $(OUTDIR)/dobetter_rop_x86.exe

.PHONY: all clean check-compiler

all: check-compiler $(TARGET)

check-compiler:
	@command -v $(MINGW_CC) >/dev/null 2>&1 || { echo "need $(MINGW_CC)"; exit 1; }

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(SRC) | $(OUTDIR)
	$(MINGW_CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "Built $@ (DEP/NX expected ON, ASLR OFF)"
	@file $@ 2>/dev/null || true
	@i686-w64-mingw32-objdump -x $@ 2>/dev/null | grep -i DllCharacteristics || true

clean:
	rm -rf $(OUTDIR)
