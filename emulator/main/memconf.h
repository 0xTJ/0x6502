uint8_t ram_1[0x400] = {0};
uint8_t ram_2[0x400] = {0};
uint8_t ram_3[0x400] = {0};
uint8_t rom_1[0x200] = {0};

const struct PageDef page_defs[] = {
    {
        .page = 0x00,
        .type = DRAM,
        .array = ram_1,
        .array_base = 0x0000,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x01,
        .type = DRAM,
        .array = ram_1,
        .array_base = 0x0100,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x02,
        .type = DRAM,
        .array = ram_1,
        .array_base = 0x0200,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x03,
        .type = DRAM,
        .array = ram_1,
        .array_base = 0x0300,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x04,
        .type = DRAM,
        .array = ram_2,
        .array_base = 0x0000,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x05,
        .type = DRAM,
        .array = ram_2,
        .array_base = 0x0100,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x06,
        .type = DRAM,
        .array = ram_2,
        .array_base = 0x0200,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x07,
        .type = DRAM,
        .array = ram_2,
        .array_base = 0x0300,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x08,
        .type = DRAM,
        .array = ram_3,
        .array_base = 0x0000,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x09,
        .type = DRAM,
        .array = ram_3,
        .array_base = 0x0100,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0A,
        .type = DRAM,
        .array = ram_3,
        .array_base = 0x0200,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x0B,
        .type = DRAM,
        .array = ram_3,
        .array_base = 0x0300,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xF0,
        .type = ROM,
        .array = rom_1,
        .array_base = 0x0000,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0xFF,
        .type = ROM,
        .array = rom_1,
        .array_base = 0x0100,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    }
};