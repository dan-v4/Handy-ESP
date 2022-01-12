#ifndef ILI9341_H
#define ILI9341_H

#include <stdio.h>
#include "driver/spi_master.h"

void ili_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

void push_color(uint32_t color);

void draw_pixel(uint8_t x, uint8_t y, uint32_t colour);

void draw_char(int16_t x, int16_t y, char c, uint32_t textColor, uint8_t size);

uint32_t draw_string_no_newline(uint16_t x, uint16_t y, char *pt, uint32_t textColor, int8_t size, size_t array_size);

void fill_screen(uint32_t textColor);

void draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t textColor);

void ili_startup();

#endif