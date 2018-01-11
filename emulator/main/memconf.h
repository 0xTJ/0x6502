uint8_t ram[0x3000] = {0};
uint8_t rom[0x100]  = {0};

const struct MemConf mem_conf[] = {
    {
        .start = 0x0000,
        .size = sizeof(ram),
        .type = RAM,
        .array = ram
    },
    {
        .start = 0xfffa,
        .size = sizeof(rom),
        .type = ROM,
        .array = rom
    },
};