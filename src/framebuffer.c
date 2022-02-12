#include "framebuffer.h"
// #include "serial.h"

uint32_t framebuffer_color(framebuffer_t *fb, uint32_t red, uint32_t green, uint32_t blue)
{
    uint32_t color = 0;
    if (red)
        color |= ((1 << fb->red_mask_size) - 1) << fb->red_field_position;
    if (green)
        color |= ((1 << fb->green_mask_size) - 1) << fb->green_field_position;
    if (blue)
        color |= ((1 << fb->blue_mask_size) - 1) << fb->blue_field_position;

    return color;
}
void framebuffer_set_pixel(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t color)
{
    if (x < fb->width && y < fb->height)
    {
        uint32_t *pixel = fb->addr + fb->pitch * y + (fb->bpp / 8) * x;
        *pixel = color;
    }
}
void framebuffer_draw_rect(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
    uint32_t *pixel = fb->addr + fb->pitch * y + (fb->bpp / 8) * x;
    for (uint32_t i = 0; i < height; i++)
    {
        for (uint32_t j = 0; j < width; j++)
        {
            *pixel = color;
            pixel++;
        }
        pixel += fb->pitch / 4 - width;
    }
}
uint32_t color(uint8_t red, uint8_t green, uint8_t blue)
{
    return (red << 16) | (green << 8) | blue;
}
void draw_xbm(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *xbm)
{
    for (uint32_t i = 0; i < width; i++)
    {
        for (uint32_t j = 0; j < height; j++)
        {
            if (xbm[j] & 1 << i)
            {
                // serial_println("1");
                framebuffer_set_pixel(fb, x + i, y + j, color(255, 255, 255));
            }
            else
            {
                // serial_println("0");
            }
        }
    }
}
