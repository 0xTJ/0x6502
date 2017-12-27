#include "emulator.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "duo_travel.h"
#include "core.h"

bool screenDisp = false;

void dispRam() {
    for (uint8_t i = 0; i < 16; i++) {
        displayCharacter(i, 0, screen_mem[i]);
    }
    for (uint8_t i = 0; i < 16; i++) {
        displayCharacter(i, 1, screen_mem[16 + i]);
    }
}

void dispProc() {
    clearDisplay();
    char buf[17];
    sprintf(buf, "A"BYTE_TO_BINARY_PATTERN" PC%04X", BYTE_TO_BINARY(a), pc);
    for (int i = 0; i < 16 && buf[i]; i++)
        displayCharacter(i, 0, buf[i]);
    sprintf(buf, "P"BYTE_TO_BINARY_PATTERN" %02X S%02X", BYTE_TO_BINARY(status), instructions & 0xFF, sp);
    for (int i = 0; i < 16 && buf[i]; i++)
        displayCharacter(i, 1, buf[i]);
}

void run() {
    resetMachine();
    reset6502();
    screenDisp = true;
    while (true) {        
        if (screenDisp)
            dispRam();
        else
            dispProc();
        
        // while (getKey() < 8);
        
        uint16_t tmp_keys = getPressedKeys();
        if (tmp_keys >> KEY_ALT & 1) {
            return;
        }
        else if (tmp_keys >> KEY_DEL & 1) {
            screenDisp = true;
        }
        else if (tmp_keys >> KEY_SAVE & 1) {
            screenDisp = false;
        }
        else {
            step6502();
        }
    }
}

uint8_t read6502(uint16_t address) {
    uint8_t page = (address >> 8) & 0xFF;
    uint8_t addr = address & 0xFF;
    
    if (defd_pages[page] != NULL) {
        switch (defd_pages[page]->type) {
            case NONE:
                return 0;
            case ROM:
            case RAM:
                return readStorageInt8(address);
                break;
            case IO:
                return defd_pages[page]->ioReadFunc(addr);
        }
    }
    else
        return 0;
}

void write6502(uint16_t address, uint8_t value) {
    uint8_t page = (address >> 8) & 0xFF;
    uint8_t addr = address & 0xFF;
    
    if (defd_pages[page] != NULL) {
        switch (defd_pages[page]->type) {
            case NONE:
                return;
            case ROM:
                if (is_running)
                    return;
            case RAM:
                writeStorage(((uint16_t) defd_pages[page]->eeprom_addr << 8) + addr, &value, 1);
                break;
            case IO:
                defd_pages[page]->ioWriteFunc(addr, value);
                return;
        }
    }
}
