#include "machine.h"
#include <stdlib.h>
#include <stdio.h>
#include "core.h"

bool is_running = false;
const struct PageDef *defd_pages[0x100];
#include "memconf.h"
const uint16_t defd_page_count = sizeof(page_defs) / sizeof(struct PageDef);

void setupMachine() {
    for (uint16_t i = 0; i < defd_page_count; i++) {
        defd_pages[page_defs[i].page] = &(page_defs[i]);
    }
    
    resetMachine();
    
    hookexternal(externProc);
}

void resetMachine() {
    for (uint16_t i = 0; i < defd_page_count; i++) {
        if (page_defs[i].type == DRAM) {
            for (uint16_t a = 0; a < 0x100; a++) {
                write6502((((uint16_t)page_defs[i].page) << 8) + a, 0);
            }
        }
        else if (page_defs[i].type == IO) {
            page_defs[i].ioInitFunc();
        }
    }
}

uint32_t next_clockticks6502 = IRQ_TUNING;

void externProc() {
    if (clockticks6502 >= next_clockticks6502) {
        next_clockticks6502 += IRQ_TUNING;
        irq6502();
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
            case DRAM:
            case SRAM:
                return defd_pages[page]->array[defd_pages[page]->array_base + addr];
                break;
            case IO:
                return defd_pages[page]->ioReadFunc(addr);
        }
    }
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
            case DRAM:
            case SRAM:
                defd_pages[page]->array[defd_pages[page]->array_base + addr] = value;
                break;
            case IO:
                defd_pages[page]->ioWriteFunc(addr, value);
                return;
        }
    }
}
