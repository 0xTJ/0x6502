#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "argtable3/argtable3.h"
#include "cmd_decl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "editor.h"
#include "emulator.h"

static void register_address();
static void register_write();
static void register_read();
static void register_hex();
static void register_bin();
static void register_run();

void register_editor()
{
    register_address();
    register_read();
    register_write();
    register_hex();
    register_bin();
    register_run();
}

static struct {
    struct arg_int *addr;
    struct arg_end *end;
} address_args;

static int address(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &address_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, address_args.end, argv[0]);
        return 1;
    }
    if (address_args.addr->count) {
        editor_addr = address_args.addr->ival[0];
    }
    return 0;
}

static void register_address()
{
    address_args.addr =
            arg_int1(NULL, NULL, "<a>", "Editor address");
    address_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "address",
        .help = "Set editor address. ",
        .hint = NULL,
        .func = &address,
        .argtable = &address_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static struct {
    struct arg_int *addr;
    struct arg_lit *format;
    struct arg_int *count;
    struct arg_end *end;
} read_args;

static int read(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &read_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, read_args.end, argv[0]);
        return 1;
    }
    uint16_t addr = editor_addr;
    if (read_args.addr->count) {
        addr = read_args.addr->ival[0];
    }
    if (read_args.count->count) {
        const int count = read_args.count->ival[0];
        if (read_args.format->count) {
            uint16_t curr_addr = addr;
            for (uint16_t i = 0; i < count; i++) {
                curr_addr = addr + i;
                if (i == 0 || !(curr_addr & 0xF)) {
                    printf("%04x:", curr_addr);
                }
                if (i == 0) {
                    for (int j = 0; j < (curr_addr & 0xF); j++) {
                        printf(" XX");
                    }
                }
                printf(" %02x", editor_read(curr_addr));
                if ((curr_addr & 0xF) == 15) {
                    printf("\n");
                }
            }
            printf("\n");
        }
        else {
            uint16_t curr_addr = addr;
            for (uint16_t i = 0; i < count; i++) {
                curr_addr = addr + i;
                printf(" %02x", editor_read(curr_addr));
            }
            printf("\n");
        }
    }
    return 0;
}

static void register_read()
{
    read_args.addr =
            arg_int0("a", "address", "<a>", "Address as write base");
    read_args.format =
            arg_lit0("f", "formatted", "Format read data");
    read_args.count =
            arg_int1(NULL, NULL, "<n>", "Count to read");
    read_args.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "read",
        .help = "Read data from memory. ",
        .hint = NULL,
        .func = &read,
        .argtable = &read_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static struct {
    struct arg_int *addr;
    struct arg_str *data;
    struct arg_end *end;
} write_args;

static int write(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &write_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, write_args.end, argv[0]);
        return 1;
    }
    uint16_t addr = editor_addr;
    if (write_args.addr->count) {
        addr = write_args.addr->ival[0];
    }
    if (write_args.data->count) {
        const char *p = write_args.data->sval[0];
        while (*p) {
            if (isdigit((unsigned char)*p)) {
                long int val = strtol(p, (char **) &p, 0);
                if (val < 0 || 0xFF < val) {
                    printf("Data must be between 0 and 0xFF (255): %ld", val);
                    return 1;
                }
                editor_write(addr, (uint8_t)val);
            } else {
                p++;
            }
        }
    }
    editor_addr = addr + 1;
    return 0;
}

static void register_write()
{
    write_args.addr =
            arg_int0("a", "address", "<a>", "Address as write base");
    write_args.data =
            arg_str1(NULL, NULL, "<data>", "String of numbers as data");
    write_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "write",
        .help = "Write data to memory. ",
        .hint = NULL,
        .func = &write,
        .argtable = &write_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static struct {
    struct arg_int *addr;
    struct arg_end *end;
} bin_args;

static int bin(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &bin_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, bin_args.end, argv[0]);
        return 1;
    }
    uint16_t addr = editor_addr;
    if (bin_args.addr->count) {
        addr = bin_args.addr->ival[0];
    }
    while (true) {
        char buf[3] = {0};
        buf[0] = fgetc(stdin);
        if (buf[0] == '\t')
            break;
        else if (!isalnum((unsigned char) buf[0]))
            continue;
        buf[1] = fgetc(stdin);
        if (buf[1] == '\t')
            break;
        editor_write(addr, strtoul(buf, NULL, 16));
        addr++;
    }
    editor_addr = addr;
    return 0;
}

static void register_bin()
{
    bin_args.addr =
            arg_int0("a", "address", "<a>", "Address as write base");
    bin_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "bin",
        .help = "Write binary to memory. ",
        .hint = NULL,
        .func = &bin,
        .argtable = &bin_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static struct {
    struct arg_end *end;
} hex_args;

static int hex(int argc, char** argv)
{
    char buf[128] = {0};
    while (true) {
        fgets(buf, sizeof(buf), stdin);
        unsigned int count, address, type, checksum;
        sscanf(buf, ":%2x%4x%2x", &count, &address, &type);
        char format[20];
        sprintf(format, "%%%ds%%2x", count * 2);
        char *data = malloc(sizeof(char) * (count * 2 + 10));
        sscanf(buf + 9, format, data, &checksum);
        if (type == 0x00) {
            printf("Writing to %04x: %s\n", address, data);
            for (unsigned int i = 0; i < count; i ++) {
                unsigned int byte;
                sscanf(data + (2 * i), "%2x", &byte);
                // printf("Writing %02x to %04x\n", byte, address + i);
                editor_write(address + i, byte);
            }
        }
        else if (type == 0x01) {
            return 0;
        }
    }
}

static void register_hex()
{
    hex_args.end = arg_end(0);

    const esp_console_cmd_t cmd = {
        .command = "hex",
        .help = "Write hex to memory. ",
        .hint = NULL,
        .func = &hex,
        .argtable = &hex_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static struct {
    struct arg_end *end;
} run_args;

static int run(int argc, char** argv)
{
    return emulator();
}

static void register_run()
{
    run_args.end = arg_end(0);

    const esp_console_cmd_t cmd = {
        .command = "run",
        .help = "Runs 6502. ",
        .hint = NULL,
        .func = &run,
        .argtable = &run_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
