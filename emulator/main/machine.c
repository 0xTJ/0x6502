#include "machine.h"
#include <stdlib.h>
#include <stdio.h>
#include "core.h"

bool is_running = false;
#include "memconf.h"
const uint16_t mem_conf_count = sizeof(mem_conf) / sizeof(struct MemConf);

void setupMachine() {    
    resetMachine();
    hookexternal(externProc);
}

void resetMachine() {
    // for (uint16_t i = 0; i < defd_page_count; i++) {
        // if (page_defs[i].type == RAM) {
            // for (uint16_t a = 0; a < 0x100; a++) {
                // write6502((((uint16_t)page_defs[i].page) << 8) + a, 0);
            // }
        // }
        // else if (page_defs[i].type == IO) {
            // page_defs[i].ioInitFunc();
        // }
    // }
}

uint32_t next_clockticks6502 = IRQ_TUNING;

void externProc() {
    if (clockticks6502 >= next_clockticks6502) {
        next_clockticks6502 += IRQ_TUNING;
        irq6502();
    }
}

static const struct MemConf *getMemConf(uint16_t address) {
    for (size_t i = 0; i < mem_conf_count; i++) {
        if (mem_conf[i].start <= address && address < (mem_conf[i].start + mem_conf[i].size))
            return mem_conf + i;
    }
    return NULL;
}

uint8_t read6502(uint16_t address) {
    const struct MemConf *this_mem_conf = getMemConf(address);
    if (this_mem_conf != NULL) {
        switch (this_mem_conf->type) {
            case NONE:
                return 0;
            case ROM:
            case RAM:
                return this_mem_conf->array[address - this_mem_conf->start];
                break;
            case IO:
                return this_mem_conf->ioReadFunc(address);
        }
    }
    return 0;
}

void write6502(uint16_t address, uint8_t value) {
    const struct MemConf *this_mem_conf = getMemConf(address);
    if (this_mem_conf != NULL) {
        switch (this_mem_conf->type) {
            case NONE:
                return;
            case ROM:
                if (is_running)
                    return;
            case RAM:
                this_mem_conf->array[address - this_mem_conf->start] = value;
                break;
            case IO:
                this_mem_conf->ioWriteFunc(address, value);
                return;
        }
    }
}

void get_keys(unsigned char c) {
    uint8_t head = read6502(KEYBRD_HEAD);
    uint8_t tail = read6502(KEYBRD_TAIL);
    uint8_t next_tail = tail + 1;
    if (next_tail > (KEYBRD_TOP - KEYBRD_BASE)) {
        next_tail = 0;
    }
    uint8_t next_head = head;
    if (next_tail == head) {
        next_head = head + 1;
        if (next_head > (KEYBRD_TOP - KEYBRD_BASE)) {
            next_head = 0;
        }
    }
    write6502(KEYBRD_BASE + tail, c);
    write6502(KEYBRD_HEAD, next_head);
    write6502(KEYBRD_TAIL, next_tail);
}
