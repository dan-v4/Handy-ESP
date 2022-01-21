# Handy-ESP

The goal of this project is to turn an ESP32 into a handheld device with basic functions. Functions such as text editor, image viewer, calculator, etc.
The device uses an SD CARD for storage and a 2.8" 240x160 pixel TFT Display which is controlled by an ILI9341. Both these devices are SPI slaves to the ESP32.
The project uses the ESP-IDF framework and is coded in C.

## Implemented

1. Basic LCD driver
2. Basic settings (change background and font colour)
3. Button handling
4. Initializing SPI bus and devices
5. SD Card writing and reading
6. Main menu animations and flow

## To be implemented

1. Text editor
2. Frame Buffer
3. File Explorer
4. Calculator
5. Expand on settings

## Future applications possibly implemented

1. Paint app
2. Emulator
