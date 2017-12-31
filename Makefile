clean:
	$(MAKE) -C asm clean
	$(MAKE) -C emulator clean
	
all:
	$(MAKE) -C asm all
	$(MAKE) -C emulator all
	
flash:
	$(MAKE) -C emulator flash