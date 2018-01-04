uint8_t ram_1[0x300] = {0};
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