# Morse Code Decoder — STM32F103

A bare-metal embedded project that decodes Morse code input from physical buttons and outputs the decoded letter over UART serial — running on an STM32F103 "Blue Pill" microcontroller.

Built using [libopencm3](https://github.com/libopencm3/libopencm3) — an open-source peripheral library for STM32.



## How It Works

1. **Hold PA0** (Start button) → LED on PC13 turns ON, session begins
2. **Tap PA1** (Morse button) → short tap = dot `.`, long tap = dash `-`
3. **Release PA0** → LED turns OFF, input is decoded and printed over serial

Dot/dash threshold: **300ms** — tap under 300ms is a dot, over is a dash.



## Hardware

| Pin  | Function              |
|------|-----------------------|
| PA0  | Start/Stop button     |
| PA1  | Morse input button    |
| PA9  | USART1 TX (serial out)|
| PC13 | Onboard LED           |

**Board:** STM32F103C8T6 "Blue Pill"  
**Clock:** 72MHz via external 8MHz HSE crystal + PLL  
**Serial:** 9600 baud, 8N1

---

## Requirements

- ARM cross-compiler: `arm-none-eabi-gcc`
- [libopencm3](https://github.com/libopencm3/libopencm3) (set up in parent directory)
- `make`
- ST-Link or similar flasher to upload to the board

---

## Build & Flash

```bash
make          # compiles morse.c → morse.elf + morse.bin
```

Flash to board using ST-Link:
```bash
st-flash write morse.bin 0x8000000
```

Or using OpenOCD:
```bash
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
        -c "program morse.elf verify reset exit"
```

---

## Serial Output

Connect to USART1 TX (PA9) with a USB-to-TTL adapter at **9600 baud**.

Example output:
```
Morse Code Ready...
.-      A
-...    B
...     S
```

---

## Morse Table

The decoder supports all 26 letters (A–Z) plus space:

| Letter | Code   | Letter | Code  |
|--------|--------|--------|-------|
| A      | `.-`   | N      | `-.`  |
| B      | `-...` | O      | `---` |
| C      | `-.-.` | P      | `.--.`|
| D      | `-..`  | Q      | `--.-`|
| E      | `.`    | R      | `.-.` |
| F      | `..-.` | S      | `...` |
| G      | `--.`  | T      | `-`   |
| H      | `....` | U      | `..-` |
| I      | `..`   | V      | `...-`|
| J      | `.---` | W      | `.--` |
| K      | `-.-`  | X      | `-..-`|
| L      | `.-..` | Y      | `-.--`|
| M      | `--`   | Z      | `--..`|

Unknown patterns output `?`.



## Project Structure


morseCode/
├── morse.c        ← main source code
├── Makefile       ← build config
├── morse.elf      ← compiled binary with debug info
├── morse.bin      ← raw binary for flashing
├── morse.map      ← memory map
├── morse.o        ← compiled object file
└── morse.d        ← dependency file
```

---
Dhanvi Vijaykumar
