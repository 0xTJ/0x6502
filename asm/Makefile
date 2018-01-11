BINARIES = myos.bin myos.hex myos.hex.h
COMPILE = ../tools/cc65/bin/ca65 --cpu 6502X
LINK = ../tools/cc65/bin/ld65

# .SUFFIXES: .s .bin .hex

all: 0x6502-main.bin 0x6502-vect.bin

0x6502.o:	0x6502.s helpers.s
	$(COMPILE) -l 0x6502.lst 0x6502.s -o 0x6502.o

0x6502-main.bin 0x6502-vect.bin: 0x6502.o
	$(LINK) -m 0x6502.mod -C 0x6502.cfg 0x6502.o -o 0x6502
    
clean:
	rm -f *.lst *.o *.bin *.mod
