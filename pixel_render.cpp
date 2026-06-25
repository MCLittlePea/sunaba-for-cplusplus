#include "pixel_render.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

static int s_width = 100;
static int s_height = 100;
static uint8_t *s_framebuffer = NULL;
static HWND s_hWnd = NULL;
static bool s_running = false;
static bool s_class_registered = false;

static void init_buffer(void) {
    if (!s_framebuffer) {
        s_framebuffer = (uint8_t *) calloc(s_width * s_height * 4, 1);
    }
}

void set_window_size(int width, int height) {
    if (s_running) return;
    if (s_framebuffer) {
        free(s_framebuffer);
        s_framebuffer = NULL;
    }
    s_width = width;
    s_height = height;
}

void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (!s_framebuffer) return;
    if (x < 0 || x >= s_width || y < 0 || y >= s_height) return;
    int idx = (y * s_width + x) * 4;
    s_framebuffer[idx + 0] = b;
    s_framebuffer[idx + 1] = g;
    s_framebuffer[idx + 2] = r;
    s_framebuffer[idx + 3] = 255;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            BITMAPINFO bmi = {0};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = s_width;
            bmi.bmiHeader.biHeight = -s_height;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            StretchDIBits(hdc, 0, 0, s_width, s_height, 0, 0, s_width, s_height,
                          s_framebuffer, &bmi, DIB_RGB_COLORS, SRCCOPY);
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcA(hWnd, msg, wParam, lParam);
    }
}

void create_window(void) {
    if (s_running) return;
    init_buffer();
    HINSTANCE hInstance = GetModuleHandleA(NULL);
    const char *class_name = "PixelRenderWindow";
    if (!s_class_registered) {
        WNDCLASSA wc = {0};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = class_name;
        wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
        RegisterClassA(&wc);
        s_class_registered = true;
    }
    RECT rc = {0, 0, s_width, s_height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    s_hWnd = CreateWindowA(class_name, "Pixel Window", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
                           NULL, NULL, hInstance, NULL);
    ShowWindow(s_hWnd, SW_SHOW);
    UpdateWindow(s_hWnd);
    s_running = true;
}

bool update_window(void) {
    if (!s_running || !s_hWnd) return false;
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            s_running = false;
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    InvalidateRect(s_hWnd, NULL, FALSE);
    UpdateWindow(s_hWnd);
    return true;
}

void destroy_window(void) {
    if (s_hWnd) {
        DestroyWindow(s_hWnd);
        s_hWnd = NULL;
    }
    s_running = false;
    if (s_framebuffer) {
        free(s_framebuffer);
        s_framebuffer = NULL;
    }
}
