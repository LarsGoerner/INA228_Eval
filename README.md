# INA228 Evaluation Project

This project is used to evaluate some of the functions of an INA228 
Power/Energy/Charge monitor.
Specifically the current and voltage sensing is used here.

## Hardware

```
  ST-Link          STM32F030F4            INA228
+---------+     +---------------+     +------------+
|         |     |               |     |            |
|  SWCLK <|>---<|> PA14    PA9 <|>---<|> SCL       |
|  SWDIO <|>---<|> PA13   PA10 <|>---<|> SDA       |
|   NRST <|>---<|> NRST    GND <|>---<|> GND       |
|         |     |               |   x<|> ALERT     |
|         |     |               |     |            |
|         |     |               |     +------------+
|         |     |               |
|         |     |               |     +------------+
|         |     |               |     |            |
|         |     |          PA2 <|>---<|> RX        |
|    VCC <|>---<|> VCC     PA3 <|>---<|> TX        |
|    GND <|>---<|> GND     GND <|>---<|> GND       |
|         |     |               |     |            |
+---------+     +---------------+     +------------+
                                        UART to USB
```

## How to use

### Used Tools

- [GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/gnu-rm)
- [OpenOCD](https://openocd.org/)
- [Make](https://en.wikipedia.org/wiki/Make_(software))

### Build Process

```bash
cd <project main directory>
make
```

### Program & Debug MCU

OpenOcd can be used to program the device.

```bash
make ocd
```

This command will open an OpenOCD instance.
From here, there are 2 possible ways to use it:

1. Using a TELNET connection for direct OpenOCD commands
2. Using gdb for debugging

#### TELNET Connection

The OpenOCD server will listen on port 4444 for TELNET communications.
Once you opened a terminal you can use the following commands to program and 
run the project:

```bash
reset init
flash write_image erase build/fw.elf
reset
```

#### Debugging

The OpenOCD server will listen on port 3333.

Run GDB:

```bash
arm-none-eabi-gdb
```

Inside GDB:

```bash
(gdb) file build/fw.elf
Reading symbols from build/fw.elf...
(gdb) target remote localhost:3333
```
