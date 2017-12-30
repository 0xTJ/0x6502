# Name: Makefile
# Author: <insert your name here>
# Copyright: <insert your copyright message here>
# License: <insert your license reference here>

# This is a prototype Makefile. Modify it according to your needs.
# You should at least check the settings for
# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected. We recommend that you leave it undefined and
#                add settings like this to your ~/.avrduderc file:
#                   default_programmer = "stk500v2"
#                   default_serial = "avrdoper"
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

DEVICE     = atmega328p
CLOCK      = 8000000
PROGRAMMER = -c usbasp
#PROGRAMMER = -c stk500v2 -P /dev/cu.usbmodem1411
OBJECTS    = main.o duo_travel.o editor.o emulator.o core.o machine.o programmer.o
# OBJECTS    = main.o duo_travel.o editor.o emulator.o core.o machine.o
# OBJECTS    = main.o duo_travel.o programmer.o machine.o
FUSES      = -U lfuse:w:0xe2:m -U efuse:w:0x04:m -U hfuse:w:0xd9:m
# avrdude -c usbasp -p atmega328p -B 2 -U flash:w:main.hex:i
# Tune the lines below only if you know what you are doing:

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
# COMPILE = C:\msys64\opt\avr-gcc\bin\avr-gcc -Wall -Wno-pointer-sign -Wno-char-subscripts -Wno-strict-aliasing -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -fstack-usage
# COMPILE = C:\msys64\opt\avr-gcc\bin\avr-gcc -Wno-pointer-sign -Wno-char-subscripts -Wno-strict-aliasing -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -fstack-usage -fshort-enums
COMPILE = C:\msys64\opt\avr-gcc\bin\avr-gcc -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -fstack-usage -fshort-enums

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -B 2 -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) -B 50 $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID main.hex

clean:
	rm -f main.hex main.elf $(OBJECTS)

# file targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	C:\msys64\opt\avr-gcc\bin\avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	C:\msys64\opt\avr-gcc\bin\avr-size main.elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	main.elf
	C:\msys64\opt\avr-gcc\bin\avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
