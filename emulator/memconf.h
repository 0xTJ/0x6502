const struct PageDef page_defs[] = {
    {
        .page = 0x00,
        .type = DRAM,
        .eeprom_addr = 0x00,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x01,
        .type = DRAM,
        .eeprom_addr = 0x01,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x03,
        .type = DRAM,
        .eeprom_addr = 0x03,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x04,
        .type = DRAM,
        .eeprom_addr = 0x04,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x05,
        .type = DRAM,
        .eeprom_addr = 0x05,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x06,
        .type = DRAM,
        .eeprom_addr = 0x06,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x07,
        .type = DRAM,
        .eeprom_addr = 0x07,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x08,
        .type = DRAM,
        .eeprom_addr = 0x08,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x09,
        .type = DRAM,
        .eeprom_addr = 0x09,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0A,
        .type = DRAM,
        .eeprom_addr = 0x0A,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0B,
        .type = DRAM,
        .eeprom_addr = 0x0B,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0C,
        .type = DRAM,
        .eeprom_addr = 0x0C,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0D,
        .type = DRAM,
        .eeprom_addr = 0x0D,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0E,
        .type = DRAM,
        .eeprom_addr = 0x0E,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0F,
        .type = DRAM,
        .eeprom_addr = 0x0F,
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
        .page = 0xF0,
        .type = ROM,
        .eeprom_addr = 0x70,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xF1,
        .type = ROM,
        .eeprom_addr = 0x71,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xF2,
        .type = ROM,
        .eeprom_addr = 0x72,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xF3,
        .type = ROM,
        .eeprom_addr = 0x73,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xF4,
        .type = ROM,
        .eeprom_addr = 0x74,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xF5,
        .type = ROM,
        .eeprom_addr = 0x75,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xF6,
        .type = ROM,
        .eeprom_addr = 0x76,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xF7,
        .type = ROM,
        .eeprom_addr = 0x77,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xF8,
        .type = ROM,
        .eeprom_addr = 0x78,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xF9,
        .type = ROM,
        .eeprom_addr = 0x79,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xFA,
        .type = ROM,
        .eeprom_addr = 0x7A,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xFB,
        .type = ROM,
        .eeprom_addr = 0x7B,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xFC,
        .type = ROM,
        .eeprom_addr = 0x7C,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xFD,
        .type = ROM,
        .eeprom_addr = 0x7D,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xFE,
        .type = ROM,
        .eeprom_addr = 0x7E,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xFF,
        .type = ROM,
        .eeprom_addr = 0x7F,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    }
};