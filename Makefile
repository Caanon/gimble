include board.mk

TARGET = main

LIBDIRS = twi uart gyro

SRC_DIR = src
BINARY_DIR = bin
BUILD_DIR = build

AVR_GCC = avr-gcc
OBJ_COPY = avr-objcopy
AVR_OBJDUMP = avr-objdump

C_FLAGS += -DF_CPU=$(F_CPU)
C_FLAGS += -Os
C_FLAGS += -g
C_FLAGS += -std=gnu99
C_FLAGS += -Wall
C_FLAGS += -Werror
C_FLAGS += -I. -I$(SRC_DIR)
# Data types
C_FLAGS += -funsigned-char
C_FLAGS += -funsigned-bitfields
C_FLAGS += -fpack-struct
C_FLAGS += -fshort-enums
# LST file
C_FLAGS += -Wa,-adhlns=$(@:.o=.lst)

LD_FLAGS += -Wl,-Map,$(basename $@).map 

# Rules

SOURCE_PATHS = $(wildcard *.c $(SRC_DIR)/*.c)
SOURCE_PATHS += $(foreach dir, $(LIBDIRS),$(wildcard *.c $(SRC_DIR)/$(dir)/*.c))

HEADERS = $(foreach dir, $(LIBDIRS),$(wildcard *.h $(SRC_DIR)/$(dir)/*.h))
HEADER_PATHS = $(foreach path, $(HEADERS), $(dir $(path)))
C_FLAGS += $(foreach dir, $(HEADER_PATHS), -I$(dir))

SOURCES = $(subst $(SRC_DIR)/,,$(SOURCE_PATHS))
OBJECTS = $(foreach source, $(SOURCES:.c=.o), $(BUILD_DIR)/$(source))

ifneq ($(V),1)
Q := @
endif

all: directories bootloader main

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BINARY_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@printf " CC $<\n"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(AVR_GCC) $(C_FLAGS) -c $< -o $@

%.elf: $(OBJECTS)
	@printf " LD $@\n"
	$(Q)$(AVR_GCC) $(C_FLAGS) $(LD_FLAGS) $^ -o $@

.SECONDARY:
%.hex: %.elf
	@printf " HEX $@\n"
	$(Q)$(OBJ_COPY) -O ihex $^ $@

%.asm: %.hex
	@printf " ASM $@\n"
	$(Q)$(AVR_OBJDUMP) -j .sec1 -d -m $(AVR_TYPE) $^ > $@

%.lss: %.elf
	@printf " LSS $@\n"
	$(Q)$(AVR_OBJDUMP) -h -S $^ > $@

main: $(OBJECTS) $(BINARY_DIR)/$(TARGET).hex $(BINARY_DIR)/$(TARGET).asm \
	$(BINARY_DIR)/$(TARGET).lss

bootloader:
	cd bootloader; \
	make mega2560

pboot: bootloader
	cd bootloader; \
	sudo avrdude -c buspirate -P /dev/ttyUSB0 -p m2560 -U flash:w:boot_main.hex -x spifreq=5

p: main
	cd bin; \
	sudo avrdude -c buspirate -P /dev/ttyUSB0 -p m2560 -U flash:w:main.hex -x spifreq=5

dump:
	@$(foreach V,$(sort $(.VARIABLES)), $(if $(filter-out environment% default automatic, $(origin $V)),$(warning $V=$($V) ($(value $V)))))

clean:
	-rm -rf $(BINARY_DIR)/*
	-rm -rf $(BUILD_DIR)/*
