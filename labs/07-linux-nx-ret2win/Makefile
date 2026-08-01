# NX on (no -z execstack), no PIE, no canary
CC     ?= gcc
CFLAGS ?= -O0 -g -Wall -Wextra -fno-stack-protector -no-pie
SRC    = src/dobetter_linux_ret2win.c
OUTDIR = bin
TARGET = $(OUTDIR)/dobetter_linux_ret2win

.PHONY: all clean

all: $(TARGET)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(SRC) | $(OUTDIR)
	$(CC) $(CFLAGS) -o $@ $<
	@file $@
	@readelf -l $@ | grep -A1 GNU_STACK || true
	@nm $@ | grep -E ' win$$|gadget_ret' || true

clean:
	rm -rf $(OUTDIR)
