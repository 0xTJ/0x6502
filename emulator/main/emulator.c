#include "emulator.h"
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "driver/uart.h"
#include "esp_vfs_dev.h"
#include "linenoise/linenoise.h"
#include "machine.h"
#include "core.h"

#define ngetc(c) (read (0, (c), 1))

#define PRINTF_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY(i)    \
    (((i) & 0x80ll) ? '1' : '0'), \
    (((i) & 0x40ll) ? '1' : '0'), \
    (((i) & 0x20ll) ? '1' : '0'), \
    (((i) & 0x10ll) ? '1' : '0'), \
    (((i) & 0x08ll) ? '1' : '0'), \
    (((i) & 0x04ll) ? '1' : '0'), \
    (((i) & 0x02ll) ? '1' : '0'), \
    (((i) & 0x01ll) ? '1' : '0')

static void print_status() {
    printf("+-----------------------------+\n");
    printf("|A:   %02x:"PRINTF_BINARY_PATTERN"|\n", a, PRINTF_BYTE_TO_BINARY(a));
    printf("|X:   %02x:"PRINTF_BINARY_PATTERN"|\n", x, PRINTF_BYTE_TO_BINARY(x));
    printf("|Y:   %02x:"PRINTF_BINARY_PATTERN"|\n", y, PRINTF_BYTE_TO_BINARY(y));
    printf("|P:   %02x:"PRINTF_BINARY_PATTERN"|\n", status, PRINTF_BYTE_TO_BINARY(status));
    printf("|SP:  %02x:"PRINTF_BINARY_PATTERN"|\n", sp, PRINTF_BYTE_TO_BINARY(sp));
    printf("|PCH: %02x:"PRINTF_BINARY_PATTERN"|\n", (pc >> 8) & 0xFF, PRINTF_BYTE_TO_BINARY((pc >> 8) & 0xFF));
    printf("|PCL: %02x:"PRINTF_BINARY_PATTERN"|\n", pc & 0xFF, PRINTF_BYTE_TO_BINARY(pc & 0xFF));
    printf("+-----------------------------+\n");
}

uint8_t screen_mem[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

static void print_screen(bool force_show) {
    bool screen_changed = force_show;
    for (unsigned int y = 0; y < SCREEN_HEIGHT; y++) {
        for (unsigned int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t c = read6502(SCREEN_BASE + (y * SCREEN_WIDTH) + x);
            if (screen_mem[y][x] != c) {
                screen_mem[y][x] = c;
                screen_changed = true;
            }
        }
    }
    if (screen_changed) {
        linenoiseClearScreen();
        printf("+----------------------------------------+\n");
        for (unsigned int y = 0; y < SCREEN_HEIGHT; y++) {
            printf("|");
            for (unsigned int x = 0; x < SCREEN_WIDTH; x++) {
                if (isalnum(screen_mem[y][x]) || ispunct(screen_mem[y][x]) || (screen_mem[y][x] == ' '))
                    putchar(screen_mem[y][x]);
                else
                    putchar(' ');
            }
            printf("|\n");
        }
        printf("+----------------------------------------+");
    }
}

int emulator() {
    resetMachine();
    reset6502();
    bool running = true;
    bool clock_manual = true;
    unsigned long long int last_disp = 0;
    while (running) {
        if (clock_manual == true) {
            int nread;
            char c;
            nread = fread(&c, 1, 1, stdin);
            switch (c) {
            case '\t':
                clock_manual = false;
                esp_vfs_dev_uart_use_nonblocking(CONFIG_CONSOLE_UART_NUM);
                uart_driver_delete(CONFIG_CONSOLE_UART_NUM);
                setvbuf(stdin, NULL, _IOFBF, 1);
                setvbuf(stdout, NULL, _IOFBF, 1);
                printf("ENTER INTERACTIVE\n");
                print_screen(true);
                last_disp = xTaskGetTickCount();
                break;
            case 'q':
            case 'Q':
                running = false;
                break;
            case 'p':
            case 'P':
                print_status();
                break;
            case 'd':
            case 'D':
                print_screen(true);
                putchar('\n');
                break;
            case 's':
            case 'S':
                step6502();
                break;
            case 'm':
            case 'M':
                step6502();
                print_status();
                break;
            case 't':
            case 'T':
                nread = fread(&c, 1, 1, stdin);
                get_keys(c);
                break;
            case 'r':
            case 'R':
                ;char read_buf[5] = {0};
                nread = fread(read_buf, 1, 4, stdin);
                printf("%04x contains %02x\n", (unsigned int) strtoul(read_buf, NULL, 16), read6502(strtoul(read_buf, NULL, 16)));
                break;
            }
        }
        else {
            int c;
            c = getchar();
            if (c == -1) {
            }
            else if (c == '\t') {
                clock_manual = true;
                setvbuf(stdin, NULL, _IONBF, 0);
                setvbuf(stdout, NULL, _IONBF, 0);
                uart_driver_install(CONFIG_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0);
                esp_vfs_dev_uart_use_driver(CONFIG_CONSOLE_UART_NUM);
                printf("EXIT INTERACTIVE\n");
                continue;
            }
            else {
                get_keys(c);
            }            
            step6502();
            if (last_disp + 2 < xTaskGetTickCount()) {
                last_disp = xTaskGetTickCount();
                print_screen(false);
            }
        }
    }
    return 0;
}
    