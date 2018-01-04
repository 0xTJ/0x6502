#include "editor.h"
#include <stdlib.h>
#include <stdio.h>
#include "machine.h"

uint16_t editor_addr;

uint8_t editor_read(uint16_t address) {
    return read6502(address);
}

void editor_write(uint16_t address, uint8_t value) {
    write6502(address, value);
}
