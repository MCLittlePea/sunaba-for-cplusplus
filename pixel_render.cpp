#include "pixel_render.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

static int s_width = 100;
static int s_height = 100;
static uint8_t* s_framebuffer = NULL;
static HWND s_hwnd = NULL;
static bool s_running = false;
static bool s_class_registered = false;
static BITMAPINFO s_bmi;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        StretchDIBits(hdc, 0, 0, s_width, s_height, 0, 0, s_width, s_height,
            s_framebuffer, &s_bmi, DIB_RGB_COLORS, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void set_window_size(int width, int height)
{
    if (s_running) return;
    s_width = width;
    s_height = height;
}

void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (!s_framebuffer) return;
    if (x < 0 || x >= s_width || y < 0 || y >= s_height) return;
    int idx = (y * s_width + x) * 4;
    s_framebuffer[idx + 2] = r;
    s_framebuffer[idx + 1] = g;
    s_framebuffer[idx + 0] = b;
}

void create_window(void)
{
    if (s_running) return;
    s_framebuffer = (uint8_t*)calloc(s_width * s_height * 4, 1);
    memset(&s_bmi, 0, sizeof(s_bmi));
    s_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    s_bmi.bmiHeader.biWidth = s_width;
    s_bmi.bmiHeader.biHeight = -s_height;
    s_bmi.bmiHeader.biPlanes = 1;
    s_bmi.bmiHeader.biBitCount = 32;
    s_bmi.bmiHeader.biCompression = BI_RGB;

    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char* class_name = "PixelRenderWindow";
    if (!s_class_registered)
    {
        WNDCLASSEXA wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXA);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = class_name;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        RegisterClassExA(&wc);
        s_class_registered = true;
    }

    RECT rect = {0, 0, s_width, s_height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    s_hwnd = CreateWindowExA(0, class_name, "Pixel Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, hInstance, NULL);
    ShowWindow(s_hwnd, SW_SHOW);
    UpdateWindow(s_hwnd);
    s_running = true;
}

bool update_window(void)
{
    if (!s_running || !s_hwnd) return false;
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            s_running = false;
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    InvalidateRect(s_hwnd, NULL, FALSE);
    UpdateWindow(s_hwnd);
    return true;
}

void destroy_window(void)
{
    if (s_hwnd)
    {
        DestroyWindow(s_hwnd);
        s_hwnd = NULL;
    }
    s_running = false;
    if (s_framebuffer)
    {
        free(s_framebuffer);
        s_framebuffer = NULL;
    }
}