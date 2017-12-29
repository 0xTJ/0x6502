#include "machine.h"
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "duo_travel.h"

bool is_running = false;
const struct PageDef *defd_pages[0x100];
const struct PageDef page_defs[] = {
    {
        .page = 0x00,
        .type = RAM,
        .eeprom_addr = 0x00,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x01,
        .type = RAM,
        .eeprom_addr = 0x01,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x03,
        .type = RAM,
        .eeprom_addr = 0x03,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x04,
        .type = RAM,
        .eeprom_addr = 0x04,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x05,
        .type = RAM,
        .eeprom_addr = 0x05,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x06,
        .type = RAM,
        .eeprom_addr = 0x06,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x07,
        .type = RAM,
        .eeprom_addr = 0x07,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x08,
        .type = RAM,
        .eeprom_addr = 0x08,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x09,
        .type = RAM,
        .eeprom_addr = 0x09,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0A,
        .type = RAM,
        .eeprom_addr = 0x0A,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0B,
        .type = RAM,
        .eeprom_addr = 0x0B,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0C,
        .type = RAM,
        .eeprom_addr = 0x0C,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0D,
        .type = RAM,
        .eeprom_addr = 0x0D,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0E,
        .type = RAM,
        .eeprom_addr = 0x0E,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0F,
        .type = RAM,
        .eeprom_addr = 0x0F,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x10,
        .type = RAM,
        .eeprom_addr = 0x10,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x11,
        .type = RAM,
        .eeprom_addr = 0x11,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x12,
        .type = RAM,
        .eeprom_addr = 0x12,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x13,
        .type = RAM,
        .eeprom_addr = 0x13,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x14,
        .type = RAM,
        .eeprom_addr = 0x14,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x15,
        .type = RAM,
        .eeprom_addr = 0x15,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x16,
        .type = RAM,
        .eeprom_addr = 0x16,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x17,
        .type = RAM,
        .eeprom_addr = 0x17,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x18,
        .type = RAM,
        .eeprom_addr = 0x18,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x19,
        .type = RAM,
        .eeprom_addr = 0x19,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x1A,
        .type = RAM,
        .eeprom_addr = 0x1A,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x1B,
        .type = RAM,
        .eeprom_addr = 0x1B,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x1C,
        .type = RAM,
        .eeprom_addr = 0x1C,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x1D,
        .type = RAM,
        .eeprom_addr = 0x1D,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x1E,
        .type = RAM,
        .eeprom_addr = 0x1E,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x1F,
        .type = RAM,
        .eeprom_addr = 0x1F,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x20,
        .type = RAM,
        .eeprom_addr = 0x20,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x21,
        .type = RAM,
        .eeprom_addr = 0x21,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x22,
        .type = RAM,
        .eeprom_addr = 0x22,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x23,
        .type = RAM,
        .eeprom_addr = 0x23,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x24,
        .type = RAM,
        .eeprom_addr = 0x24,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x25,
        .type = RAM,
        .eeprom_addr = 0x25,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x26,
        .type = RAM,
        .eeprom_addr = 0x26,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x27,
        .type = RAM,
        .eeprom_addr = 0x27,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x28,
        .type = RAM,
        .eeprom_addr = 0x28,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x29,
        .type = RAM,
        .eeprom_addr = 0x29,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x2A,
        .type = RAM,
        .eeprom_addr = 0x2A,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x2B,
        .type = RAM,
        .eeprom_addr = 0x2B,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x2C,
        .type = RAM,
        .eeprom_addr = 0x2C,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x2D,
        .type = RAM,
        .eeprom_addr = 0x2D,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x2E,
        .type = RAM,
        .eeprom_addr = 0x2E,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x2F,
        .type = RAM,
        .eeprom_addr = 0x2F,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x30,
        .type = RAM,
        .eeprom_addr = 0x30,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x31,
        .type = RAM,
        .eeprom_addr = 0x31,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x32,
        .type = RAM,
        .eeprom_addr = 0x32,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x33,
        .type = RAM,
        .eeprom_addr = 0x33,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x34,
        .type = RAM,
        .eeprom_addr = 0x34,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x35,
        .type = RAM,
        .eeprom_addr = 0x35,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x36,
        .type = RAM,
        .eeprom_addr = 0x36,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x02,
        .type = IO,
        .eeprom_addr = 0,
        .ioInitFunc = p02Init,
        .ioReadFunc = p02Read,
        .ioWriteFunc = p02Write
    },
    {
        .page = 0xFF,
        .type = ROM,
        .eeprom_addr = 0x7F,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    }
};
const uint16_t defd_page_count = sizeof(page_defs) / sizeof(struct PageDef);

void loadPageDefs() {
    clearDisplay();
    for (uint16_t i = 0; i < defd_page_count; i++) {
        defd_pages[page_defs[i].page] = &(page_defs[i]);
    }
    resetMachine();
}

void resetMachine() {
    for (uint16_t i = 0; i < defd_page_count; i++) {
        if (page_defs[i].type == RAM) {
            // for (uint16_t a = 0; a < 0x100; a += 0x10) {
                // uint8_t tmp[0x10] = { 0 };
                // writeStorage(((uint16_t) page_defs[i].eeprom_addr << 8) + a, tmp, 0x10);
            // }
        }
        else if (page_defs[i].type == IO) {
            page_defs[i].ioInitFunc();
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

uint8_t screen_mem[32];
uint8_t sprite_mem[8 * 8];

static void updateSprite(uint8_t num) {
    sendDisplayCommand(0x40 + (num * 8));
    for (uint8_t i = 0; i < 8; i++) {
        sendDisplayCharacter(sprite_mem[(num * 8) + i]);
    }
}

void p02Init() {
    for (uint8_t i = 0; i < 32; i++) {
        screen_mem[i] = 0;
    }
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            sprite_mem[i * 8 + j] = 0x00;
        }
        updateSprite(i);
    }
}

uint8_t p02Read(uint8_t addr) {
    if ((SCREEN_BASE & 0xFF) <= addr && addr <= (SCREEN_BASE + (SCREEN_SIZE - 1) & 0xFF)) {
        return screen_mem[addr - (SCREEN_BASE & 0xFF)];
    }
    else if ((SPRITE_BASE & 0xFF) <= addr && addr <= ((SPRITE_BASE + (SPRITE_SIZE - 1)) & 0xFF)) {
        uint8_t sprite = (addr - (SPRITE_BASE & 0xFF)) / 8;
        uint8_t byte = addr - (SPRITE_BASE & 0xFF) - (sprite * 8);
        return sprite_mem[sprite * 8 + byte];
    }
    else if (addr == (KEYS_ADDR & 0xFF)) {
        return getPressedKeys() & 0xFF;
    }
}

void p02Write(uint8_t addr, uint8_t value) {
    if ((SCREEN_BASE & 0xFF) <= addr && addr <= (SCREEN_BASE + (SCREEN_SIZE - 1) & 0xFF)) {
        screen_mem[addr - (SCREEN_BASE & 0xFF)] = value;
    }
    else if ((SPRITE_BASE & 0xFF) <= addr && addr <= ((SPRITE_BASE + (SPRITE_SIZE - 1)) & 0xFF)) {
        uint8_t sprite = (addr - (SPRITE_BASE & 0xFF)) / 8;
        uint8_t byte = addr - (SPRITE_BASE & 0xFF) - (sprite * 8);
        sprite_mem[sprite * 8 + byte] = value;
        updateSprite(sprite);
    }
    else if (addr == (KEYS_ADDR & 0xFF)) {
    }
}
