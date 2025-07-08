# Arduino OLED Display Controller

This directory contains the Arduino code for controlling the CFAL12856A0-0151-B OLED display using an Arduino.

## Files

- `CFAL12856A00151B.ino` - Main Arduino sketch
- `font5x8.h` - Font bitmap data stored in PROGMEM

## Features

- 5x8 bitmap font for text rendering
- Support for special symbols (arrows, degree symbol)
- Memory-optimized for Arduino Uno
- Text positioning with x,y coordinates
- Multi-line text support
- Text auto-wrapping and scrolling
- Serial command interface
- Bitmap display support

## Serial Commands

The Arduino sketch accepts the following commands via serial:

- `text:message` - Display text at default position
- `textxy:x,y,message` - Display text at specific position
- `multiline:line1\nline2` - Display multi-line text
- `scroll:message` - Display scrolling text
- `directions` - Show direction arrows demo
- `weather` - Show weather with degree symbol demo
- `aiming` - Show aiming reticle bitmap
- `eagle` - Show eagle bitmap
- `clear` - Clear the display
- `reset` - Reset the display
- `show_logo` - Show logo
- `show_index` - Show index
- `demo` - Run demo sequence
- `help` - Show help

## Special Characters

The following escape sequences are supported in text commands:

- `\\up` - Up arrow ↑
- `\\down` - Down arrow ↓
- `\\left` - Left arrow ←
- `\\right` - Right arrow →
- `\\deg` - Degree symbol °

## Wiring

```
Arduino       OLED Display
-------       ------------
GND    -----> GND
3.3V   -----> VCC
D13    -----> SCK
D11    -----> MOSI
D10    -----> CS
D9     -----> D/C
D8     -----> RESET
```
