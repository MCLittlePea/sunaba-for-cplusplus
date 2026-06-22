#include "pixel_render.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

static int s_width = 100;
static int s_height = 100;
static uint8_t* s_framebuffer = NULL;
static HWND s_hWnd = NULL;
static HANDLE s_thread = NULL;
static bool s_running = false;
static bool s_class_registered = false;
static CRITICAL_SECTION s_cs;

static void init_buffer(void)
{
    if (!s_framebuffer)
    {
        s_framebuffer = (uint8_t*)calloc(s_width * s_height * 4, 1);
    }
}

void set_window_size(int width, int height)
{
    if (s_running) return;
    if (s_framebuffer)
    {
        free(s_framebuffer);
        s_framebuffer = NULL;
    }
    s_width = width;
    s_height = height;
}

void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (!s_framebuffer) return;
    if (x < 0 || x >= s_width || y < 0 || y >= s_height) return;

    EnterCriticalSection(&s_cs);
    int idx = (y * s_width + x) * 4;
    s_framebuffer[idx + 0] = b;
    s_framebuffer[idx + 1] = g;
    s_framebuffer[idx + 2] = r;
    s_framebuffer[idx + 3] = 255;
    LeaveCriticalSection(&s_cs);
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        BITMAPINFO bmi = {0};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = s_width;
        bmi.bmiHeader.biHeight = -s_height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        EnterCriticalSection(&s_cs);
        StretchDIBits(hdc, 0, 0, s_width, s_height, 0, 0, s_width, s_height,
            s_framebuffer, &bmi, DIB_RGB_COLORS, SRCCOPY);
        LeaveCriticalSection(&s_cs);

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

static DWORD WINAPI WindowThread(LPVOID)
{
    HINSTANCE hInstance = GetModuleHandleA(NULL);
    const char* class_name = "PixelRenderWindow";

    if (!s_class_registered)
    {
        WNDCLASSA wc = {0};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = class_name;
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
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

    MSG msg;
    while (s_running)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                s_running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // 自动刷新画面，约60帧
        InvalidateRect(s_hWnd, NULL, FALSE);
        UpdateWindow(s_hWnd);
        Sleep(16);
    }

    DestroyWindow(s_hWnd);
    s_hWnd = NULL;
    return 0;
}

void create_window(void)
{
    if (s_running) return;

    init_buffer();
    InitializeCriticalSection(&s_cs);
    s_running = true;

    s_thread = CreateThread(NULL, 0, WindowThread, NULL, 0, NULL);
}

void destroy_window(void)
{
    if (!s_running) return;

    s_running = false;
    WaitForSingleObject(s_thread, INFINITE);
    CloseHandle(s_thread);
    s_thread = NULL;

    DeleteCriticalSection(&s_cs);

    if (s_framebuffer)
    {
        free(s_framebuffer);
        s_framebuffer = NULL;
    }
}