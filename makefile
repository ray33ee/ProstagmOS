# Target architecture
TARGET = i686-elf

# C compiler
GCC = $(HOME)/opt/cross/bin/$(TARGET)-gcc
GCC_OPTIONS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# Assembler
AS = $(HOME)/opt/cross/bin/$(TARGET)-as

OS_NAME = ProstagmOS

# Grub tools
GRUB_VERIFY = grub-file --is-x86-multiboot

GRUB_RESCUE = grub-mkrescue

GRUB_CONFIG = iso/boot/grub/grub.cfg

# Chosen emulator
EMULATOR = qemu-system-i386

# All folders
BIN_FOLDER = bin
SRC_FOLDER = src
INC_FOLDER = inc
OBJ_FOLDER = obj

# Get lists of asm and c source files
ASSEMBLY_FILES := $(wildcard $(SRC_FOLDER)/*.s)
C_FILES := $(wildcard $(SRC_FOLDER)/*.c)

# Get a list of all object files
OBJ_FILES = $(patsubst $(SRC_FOLDER)/%.s, $(OBJ_FOLDER)/%.o, $(ASSEMBLY_FILES))
OBJ_FILES += $(patsubst $(SRC_FOLDER)/%.c, $(OBJ_FOLDER)/%.o, $(C_FILES))

# Linker script
LINKER = linker.ld

build: $(BIN_FOLDER)/$(OS_NAME).iso
	echo $(OBJ_FILES)

rebuild: clean build

run: $(BIN_FOLDER)/$(OS_NAME).iso
	$(EMULATOR) -cdrom $(BIN_FOLDER)/$(OS_NAME).iso

clean: 
	rm obj/*
	rm bin/*
	
$(OBJ_FOLDER)/%.o: $(SRC_FOLDER)/%.s
	$(AS) $< -o $@

$(OBJ_FOLDER)/%.o: $(SRC_FOLDER)/%.c
	$(GCC) -c $< -o $@ $(GCC_OPTIONS)

$(BIN_FOLDER)/$(OS_NAME).bin: $(OBJ_FILES) $(LINKER)
	$(GCC) -T $(LINKER) -o $(BIN_FOLDER)/$(OS_NAME).bin -ffreestanding -O2 -nostdlib $(OBJ_FILES) -lgcc
	$(GRUB_VERIFY) $(BIN_FOLDER)/$(OS_NAME).bin

$(BIN_FOLDER)/$(OS_NAME).iso: $(BIN_FOLDER)/$(OS_NAME).bin $(GRUB_CONFIG)
	cp $(BIN_FOLDER)/$(OS_NAME).bin iso/boot/$(OS_NAME).bin
	$(GRUB_RESCUE) -o $(BIN_FOLDER)/$(OS_NAME).iso iso
	


