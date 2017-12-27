#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "duo_travel.h"
#include "machine.h"
#include "emulator.h"
#include "editor.h"

int main(void) {
    init();
    loadPageDefs();
    editor();
    return 0;
}
