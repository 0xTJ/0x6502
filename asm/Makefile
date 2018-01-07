BINARIES = myos.bin myos.hex myos.hex.h
COMPILE = ../tools/cc65/bin/ca65
LINK = ../tools/cc65/bin/ld65

# .SUFFIXES: .s .bin .hex

all: 0x6502.bin

.s.o:
	$(COMPILE) -l $*.lst $^ -o $@

0x6502.bin: 0x6502.o
	$(LINK) -m 0x6502.mod -C 0x6502.ld $^ -o $@
    
clean:
	rm -f *.lst *.o *.bin *.mod
