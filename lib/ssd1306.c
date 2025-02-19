#include "ssd1306.h"
#include "ssd1306_font.h"

void ssd1306_clear(ssd1306_t *ssd) {
    for (int i = 0; i < ssd->width * ssd->height / 8; i++) {
        ssd->ram_buffer[i] = 0x00;
    }
}

void ssd1306_draw_square(ssd1306_t *ssd, uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    for (uint8_t i = 0; i < width; i++) {
        for (uint8_t j = 0; j < height; j++) {
            ssd1306_draw_pixel(ssd, x + i, y + j, 1);
        }
    }
}

void ssd1306_show(ssd1306_t *ssd) {
    for (uint8_t i = 0; i < (ssd->height / 8); i++) {
        ssd1306_send_command(0xB0 + i);
        ssd1306_send_command(0x00);
        ssd1306_send_command(0x10);
        ssd1306_write_data(&ssd->ram_buffer[ssd->width * i], ssd->width);
    }
}