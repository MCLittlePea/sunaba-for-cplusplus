#include "sunaba-for-cplusplus.h"
#include <windows.h>

int main()
{
    create_window();
    int offset = 0;
    while (update_window())
    {
        for (int x = 0; x < 100; x++)
            for (int y = 0; y < 100; y++)
                set_pixel(x, y, 0, 0, 0);
        int barX = offset % 110 - 5;
        for (int y = 0; y < 100; y++)
        {
            set_pixel(barX, y, 255, 0, 0);
            set_pixel(barX + 1, y, 0, 255, 0);
            set_pixel(barX + 2, y, 0, 0, 255);
        }
        offset++;
        Sleep(16);
    }
    destroy_window();
    return 0;
}