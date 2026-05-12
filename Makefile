# Makefile for Leslie's RTOS (ARM Cortex-R52)

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump

# Directories
SRC_KERNEL = kernel
SRC_ARCH = arch/arm/cpu/cortex_r52
SRC_LIBS = lib
SRC_USER = user

# Include path
INC = -Iinclude -I$(SRC_ARCH) -I$(SRC_KERNEL) -I$(SRC_USER)

# Architecture flags
ARCH_CFLAGS = -mcpu=cortex-r52 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16
ARCH_CFLAGS += -ffreestanding -nostdinc -fno-builtin
ARCH_CFLAGS += -Wall -Wextra -std=c99
ARCH_CFLAGS += -g

ARCH_ASFLAGS = -mcpu=cortex-r52 -mthumb

LDSCRIPT = ld/cortex_r52.ld

# Source files
KERNEL_SRC = $(SRC_KERNEL)/kernel.c $(SRC_KERNEL)/task.c \
             $(SRC_KERNEL)/sched.c $(SRC_KERNEL)/irq.c

ARCH_SRC = $(SRC_ARCH)/gic.c $(SRC_ARCH)/systick.c

LIB_SRC = lib/string.c lib/printf.c

USER_SRC = $(SRC_USER)/system_monitor.c $(SRC_USER)/temperature_sensor.c \
           $(SRC_USER)/heartbeat.c $(SRC_USER)/uart_console.c

ASM_SRC = $(SRC_ARCH)/startup.s

# Objects
KERNEL_OBJ = $(KERNEL_SRC:.c=.o)
ARCH_OBJ = $(ARCH_SRC:.c=.o)
LIB_OBJ = $(LIB_SRC:.c=.o)
USER_OBJ = $(USER_SRC:.c=.o)
ASM_OBJ = $(ASM_SRC:.s=.o)

ALL_OBJ = $(KERNEL_OBJ) $(ARCH_OBJ) $(LIB_OBJ) $(USER_OBJ) $(ASM_OBJ)

TARGET = rtos.elf
BIN = rtos.bin

.PHONY: all clean info

all: info $(TARGET) $(BIN)

info:
	@echo "=========================================="
	@echo "   Leslie's RTOS for ARM Cortex-R52"
	@echo "=========================================="
	@echo "CC: $(CC)"
	@echo "Target: $(TARGET)"
	@echo ""

$(TARGET): $(ALL_OBJ)
	@mkdir -p build
	$(LD) -T $(LDSCRIPT) $^ -o $@ -Map build/rtos.map
	@echo "Linked: $@"

%.o: %.c
	$(CC) $(ARCH_CFLAGS) $(INC) -c $< -o $@

%.o: %.s
	$(AS) $(ARCH_ASFLAGS) $< -o $@

$(BIN): $(TARGET)
	$(OBJCOPY) -O binary $< $@
	@echo "Binary: $@"

disasm: $(TARGET)
	$(OBJDUMP) -d $< > build/rtos.dis

sym: $(TARGET)
	$(OBJDUMP) -t $< > build/rtos.sym

clean:
	rm -f $(ALL_OBJ) $(TARGET) $(BIN)
	rm -f build/*

run:
	@echo "Run with QEMU:"
	@echo "  qemu-system-arm -M virt -cpu cortex-r52 -kernel $(BIN) -nographic"
	@echo ""
	@echo "Or with OpenOCD + GDB for real hardware"