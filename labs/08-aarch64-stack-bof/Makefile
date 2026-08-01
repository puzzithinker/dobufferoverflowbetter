# Native on ARM64, or cross from x86_64:
#   sudo apt install gcc-aarch64-linux-gnu
#   make

UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),aarch64)
  CC ?= gcc
else
  CC ?= aarch64-linux-gnu-gcc
endif

CFLAGS ?= -O0 -g -Wall -Wextra -fno-stack-protector -z execstack -no-pie
SRC    = src/dobetter_aarch64.c
OUTDIR = bin
TARGET = $(OUTDIR)/dobetter_aarch64

.PHONY: all clean check-compiler

all: check-compiler $(TARGET)

check-compiler:
	@command -v $(CC) >/dev/null 2>&1 || { \
	  echo "error: $(CC) not found"; \
	  echo "  sudo apt install gcc-aarch64-linux-gnu"; \
	  echo "  or build on a native aarch64 host"; \
	  exit 1; \
	}

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(SRC) | $(OUTDIR)
	$(CC) $(CFLAGS) -o $@ $<
	@file $@
	@echo "Run on aarch64 (or qemu-aarch64 ./bin/dobetter_aarch64)"

clean:
	rm -rf $(OUTDIR)
