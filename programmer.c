#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include "duo_travel.h"
#include "machine.h"
#include "rom.hex.h"

void programmer() {
    uint8_t buf[17];
    for (uint32_t i = 0; i < (sizeof(rom_hex) / sizeof(struct RomHex)); i++) {
        clearDisplay();
        sprintf(buf, "Entry %d", i);
        for (uint8_t j = 0; j < 16 && buf[j]; j++)
            displayCharacter(j, 0, buf[j]);
        
        const struct RomHex *entry = &rom_hex[i];
        for (uint8_t j = 0; j < (uint16_t) entry->byte_count; j++) {
            writeStorage(entry->address + j, &entry->data[j], 1);
            _delay_ms(1000);
        }
    }
    
    sprintf(buf, "WROTE ROM");
    for (int i = 0; i < 16 && buf[i]; i++)
        displayCharacter(i, 0, buf[i]);
}
