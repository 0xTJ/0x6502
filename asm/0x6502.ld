MEMORY {
    RAM1:  start = $0000, size = $400;
    ROM1:  start = $F000, size = $100;
    ROM2:  start = $FF00, size = $100;
}

SEGMENTS {
    CODE:       load = ROM1, type = ro;
    RODATA:     load = ROM1, type = ro;
    DATA:       load = ROM1, run = RAM1, type = rw, define = yes;
    BSS:        load = RAM1, type = bss, define = yes;
    ZEROPAGE:   load = RAM1, type = zp;
}
