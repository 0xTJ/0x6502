#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include "duo_travel.h"
#include "machine.h"
#include "../asm/myos.hex.h"

void programmer() {
    uint8_t buf[17];
    for (uint32_t i = 0; i < (sizeof(rom_hex) / sizeof(struct RomHex)); i++) {
        clearDisplay();
        sprintf(buf, "Entry %d", i + 1);
        for (uint8_t j = 0; j < 16 && buf[j]; j++)
            displayCharacter(j, 0, buf[j]);
        sprintf(buf, "of %d", sizeof(rom_hex) / sizeof(struct RomHex));
        for (uint8_t j = 0; j < 16 && buf[j]; j++)
            displayCharacter(j, 1, buf[j]);
        
        const struct RomHex *entry = &rom_hex[i];
        
        uint8_t record_type = pgm_read_byte(&entry->record_type);
        switch (record_type) {
            case 0x00:
                ;uint8_t byte_count = pgm_read_byte(&entry->byte_count);
                uint16_t base_address = pgm_read_word(&entry->address);
                for (uint16_t j = 0; j < byte_count; j++) {
                    write6502(base_address + j, (pgm_read_byte((uint8_t *) pgm_read_word(&entry->data) + j)));
                }
                break;
            case 0x01:
                return;
        }
    }
    
    clearDisplay();
    sprintf(buf, "WROTE ROM");
    for (int i = 0; i < 16 && buf[i]; i++)
        displayCharacter(i, 0, buf[i]);
}
