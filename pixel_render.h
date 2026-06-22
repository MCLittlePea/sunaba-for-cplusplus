#ifndef PIXEL_RENDER_H
#define PIXEL_RENDER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 可选：自定义窗口尺寸，必须在 create_window 之前调用
void set_window_size(int width, int height);

// 核心接口1：修改单个像素颜色，色值 0~255，越界自动忽略
// 窗口运行中可随时调用，修改后自动生效
void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);

// 核心接口2：创建并显示窗口，非阻塞，调用后立即返回
// 窗口在后台自动运行、自动刷新，无需任何额外操作
void create_window(void);

// 销毁窗口，释放所有资源，程序退出前调用
void destroy_window(void);

#ifdef __cplusplus
}
#endif

#endif // PIXEL_RENDER_H