BINARIES = myos.bin myos.hex myos.hex.h
COMPILE = ../tools/as65 -l -m -w -h0 -u -c

.SUFFIXES: .a65 .bin .hex

# symbolic targets:
all:    myos.hex

.a65.hex:
	$(COMPILE) -s2 $<

.a65.bin:
	$(COMPILE) $<

%.hex.h: %.hex
	py create_hex.py $<
    
clean:
	rm -f $(BINARIES) *.lst *.dbg
