# Convenience targets. Lab MinGW Makefiles use MINGW_CC (not host CC).

.PHONY: all labs x86 x64 linux seh rop rop64 linux-ret2win aarch64 meta clean mirror-mona help

all: labs

labs: x86 x64

x86:
	$(MAKE) -C labs/01-x86-stack-bof

x64:
	$(MAKE) -C labs/02-x64-stack-bof

linux:
	$(MAKE) -C labs/03-linux-stack-bof

seh:
	$(MAKE) -C labs/04-x86-seh-bof

rop:
	$(MAKE) -C labs/05-x86-dep-rop

rop64:
	$(MAKE) -C labs/06-x64-dep-rop

linux-ret2win:
	$(MAKE) -C labs/07-linux-nx-ret2win

aarch64:
	$(MAKE) -C labs/08-aarch64-stack-bof

meta:
	python3 scripts/extract_lab_meta.py --write labs/shared/mingw-offset-appendix.generated.md

clean:
	$(MAKE) -C labs/01-x86-stack-bof clean
	$(MAKE) -C labs/02-x64-stack-bof clean
	$(MAKE) -C labs/03-linux-stack-bof clean
	$(MAKE) -C labs/04-x86-seh-bof clean
	$(MAKE) -C labs/05-x86-dep-rop clean
	$(MAKE) -C labs/06-x64-dep-rop clean
	$(MAKE) -C labs/07-linux-nx-ret2win clean
	-$(MAKE) -C labs/08-aarch64-stack-bof clean
	-$(MAKE) -C labs/03-linux-stack-bof -f Makefile.nx clean

mirror-mona:
	./setup/scripts/Mirror-Mona3.sh

help:
	@echo "make / make linux / make seh / make rop / make rop64"
	@echo "make linux-ret2win / make aarch64 (needs aarch64-linux-gnu-gcc)"
	@echo "make meta   - regenerate labs/shared/mingw-offset-appendix.generated.md"
	@echo "See BUILDING.md ROADMAP.md"
