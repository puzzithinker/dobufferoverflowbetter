# Native Linux ELF, mitigations OFF for teaching.
#   make
#   ./bin/dobetter_linux
#   ./bin/dobetter_linux --serve 31339

CC     ?= gcc
CFLAGS ?= -O0 -g -Wall -Wextra -fno-stack-protector -z execstack -no-pie
LDFLAGS ?=

SRC    = src/dobetter_linux.c
OUTDIR = bin
TARGET = $(OUTDIR)/dobetter_linux

.PHONY: all clean checksec-hint

all: $(TARGET)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(SRC) | $(OUTDIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "Built $@"
	@file $@
	@echo "Expect: no canary, NX off (execstack), PIE off — verify with checksec if installed."

clean:
	rm -rf $(OUTDIR)

checksec-hint:
	@command -v checksec >/dev/null && checksec --file=$(TARGET) || \
	  echo "Install checksec (optional) or: readelf -l $(TARGET) | grep GNU_STACK"
