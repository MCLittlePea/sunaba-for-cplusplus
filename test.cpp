#include "sunaba-for-cplusplus.hpp"
#include <windows.h>

int main()
{
    SunabaProgram prog;
    prog.show();
    int offset = 0;
    while (true)
    {
        for (int y = 0; y < 100; ++y)
            for (int x = 0; x < 100; ++x)
                prog[60000 + y * 100 + x] = 0;

        int barX = offset % 110 - 5;
        for (int y = 0; y < 100; ++y)
        {
            prog[60000 + y * 100 + barX] = 990000;
            prog[60000 + y * 100 + barX + 1] = 9900;
            prog[60000 + y * 100 + barX + 2] = 99;
        }
        offset++;
        Sleep(16);
    }
    return 0;
}