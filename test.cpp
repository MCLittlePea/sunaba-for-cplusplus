#include "sunaba-for-cplusplus.h"
#include <windows.h>

int main()
{
    // 1. 创建窗口（非阻塞，立即返回）
    create_window();

    // 2. 窗口运行中实时修改像素：演示彩色横向移动条
    for (int offset = 0; offset < 200; offset++)
    {
        // 先清空为黑色
        for (int x = 0; x < 100; x++)
            for (int y = 0; y < 100; y++)
                set_pixel(x, y, 0, 0, 0);

        // 绘制一个宽度为10的移动彩色竖条
        int barX = offset % 110 - 5;
        for (int y = 0; y < 100; y++)
        {
            set_pixel(barX, y, 255, 0, 0);
            set_pixel(barX + 1, y, 0, 255, 0);
            set_pixel(barX + 2, y, 0, 0, 255);
        }

        // 控制动画速度，约60帧
        Sleep(16);
    }

    // 3. 销毁窗口，释放资源
    destroy_window();
    return 0;
}