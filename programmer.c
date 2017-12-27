#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include "duo_travel.h"
#include "machine.h"
#include "rom.hex.h"

#define ROM_BLOCKS (0x10)
#define EEPROM_ADDR (0x7000)
#define ROM_SIZE (0x1000)   // Size must be ROM_BLOCKS-aligned.

void programmer() {
    char buf[17];
    
    for (uint32_t a = 0; a < ROM_SIZE; a += ROM_BLOCKS) {
        sprintf(buf, "%03Xx %03Xx", a >> 4, (EEPROM_ADDR + a) >> 4);
        for (int i = 0; i < 16 && buf[i]; i++)
            displayCharacter(i, 0, buf[i]);
        
        uint8_t tmp[ROM_BLOCKS];
        for (uint16_t i = 0; i < ROM_BLOCKS; i++)
            tmp[i] = pgm_read_byte(rom + a + i);
        writeStorage(EEPROM_ADDR + a, tmp, ROM_BLOCKS);
    }
    
    sprintf(buf, "WROTE ROM");
    for (int i = 0; i < 16 && buf[i]; i++)
        displayCharacter(i, 0, buf[i]);
}
