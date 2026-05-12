# Leslie's RTOS

A real-time operating system (RTOS) designed for ARM Cortex-R52.

## Features

- Priority-based Round-Robin scheduling
- Task management (create, delete, suspend, resume)
- ARM GICv2 interrupt controller support
- System tick timer (Cortex-R52 Systick)
- Memory protection and stack management
- Minimal libc implementation (printf, string, malloc)

## Project Structure

```
leslies-rtos/
├── arch/
│   └── arm/
│       └── cpu/
│           └── cortex_r52/    # ARM Cortex-R52 specific code
│               ├── arch.h      # Architecture definitions
│               ├── gic.c       # GIC interrupt controller
│               ├── systick.c   # System tick timer
│               └── startup.s   # Bootloader and vectors
├── board/                      # Board support package
├── include/                    # Common headers
│   ├── types.h
│   ├── list.h
│   ├── task.h
│   ├── sched.h
│   ├── irq.h
│   └── systick.h
├── kernel/                     # Core kernel
│   ├── kernel.c                # Main entry
│   ├── task.c                 # Task management
│   ├── sched.c                 # Scheduler
│   └── irq.c                  # IRQ handling
├── ld/                         # Linker scripts
│   └── cortex_r52.ld
├── lib/                        # Libraries
│   ├── string.c
│   └── printf.c
└── Makefile
```

## Build

Requires ARM cross-compiler (`arm-none-eabi-gcc`):

```bash
make
```

Output:
- `rtos.elf` - ELF binary with debug info
- `rtos.bin` - Raw binary for flashing

## Idle Task

The system runs an idle task at lowest priority (255) that:
- Counts system ticks
- Periodically prints status
- Yields CPU to other tasks

## License

MIT