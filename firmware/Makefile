##########################################################################################################################
# File: Makefile
# Project: AERO-SONAR Adaptive Sonar Transmitter Payload
# Target: STM32G474RET6 (ARM Cortex-M4F @ 170 MHz with Hardware CORDIC)
# Team: Team AEROS (Team ID: 186870) | SIH 2026 (PS: 26058)
##########################################################################################################################

TARGET = aero_sonar_g474
DEBUG = 1
OPT = -O2

BUILD_DIR = build

# C sources
C_SOURCES =  \
Src/main.c \
Src/acoustic_models.c \
Src/cordic_waveform_synth.c \
Src/ewa_decision_engine.c

# Binaries
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# MCU flags
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# Macros
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32G474xx

# Includes
C_INCLUDES =  \
-IInc

# Compiler flags
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

# Libraries
LIBS = -lc -lm -lnosys
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -TSTM32G474RETx_FLASH.ld $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# Default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# List of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-ghs="$@.lst" $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir -p $@

clean:
	-rm -fR $(BUILD_DIR)

flash: all
	openocd -f interface/stlink.cfg -f target/stm32g4x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

.PHONY: all clean flash
