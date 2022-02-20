#pragma once
#include <stdint.h>
typedef struct
{
    void *addr;

    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;

    uint8_t red_mask_size;
    uint8_t red_field_position;
    uint8_t green_mask_size;
    uint8_t green_field_position;
    uint8_t blue_mask_size;
    uint8_t blue_field_position;
} framebuffer_t;

uint32_t framebuffer_color(framebuffer_t *fb, uint32_t red, uint32_t green, uint32_t blue);
void framebuffer_set_pixel(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t color);
void framebuffer_draw_rect(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
uint32_t color(uint8_t red, uint8_t green, uint8_t blue);
void draw_xbm(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *xbm);
void draw_xbm_scaled(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *xbm, uint32_t scale);