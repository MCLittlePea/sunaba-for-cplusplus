#ifndef PIXEL_RENDER_H
#define PIXEL_RENDER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void set_window_size(int width, int height);
void set_scale(int scale);
void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
void create_window(void);
bool update_window(void);
void destroy_window(void);

#ifdef __cplusplus
}
#endif

#endif