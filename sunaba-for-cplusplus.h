#ifndef SUNABA_FOR_CPLUSPLUS_H
#define SUNABA_FOR_CPLUSPLUS_H

#include "pixel_render.h"

class SunabaProgram
{
private:
    static constexpr int MXmem = 70000;
    int mem[MXmem];

public:
    SunabaProgram() = default;
    SunabaProgram(const SunabaProgram&) = default;
    SunabaProgram& operator=(const SunabaProgram&) = default;
    ~SunabaProgram() = default;

    int& operator[](int i)
    {
        ;
    }
};

#endif