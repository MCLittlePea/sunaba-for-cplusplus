#ifndef SUNABA_FOR_CPLUSPLUS_H
#define SUNABA_FOR_CPLUSPLUS_H

#include "pixel_render.h"
#include <stdexcept>
#include <windows.h>

class SunabaProgram
{
private:
    static constexpr int MXmem = 70000;
    int mem[MXmem];

    enum class Permission
    {
        ReadWrite,
        ReadOnly,
        WriteOnly,
        NoAccess
    };

    Permission get_permission(int i) const;

    class AccessProxy
    {
    private:
        int* m_ptr;
        Permission m_perm;
        int m_idx;
        SunabaProgram* m_self;
    public:
        AccessProxy(int* ptr, Permission perm, int idx, SunabaProgram* self);
        operator int() const;
        AccessProxy& operator=(int value);
    };

    static DWORD WINAPI thread_proc(LPVOID lpParam);
    void run_window_loop();
    void sync_pixels();
    void trigger_sync();

    HANDLE m_thread = nullptr;
    bool m_running = false;
    bool m_auto_sync = true;
    bool m_need_sync = false;
    CRITICAL_SECTION m_cs;

public:
    SunabaProgram() = default;
    SunabaProgram(const SunabaProgram&) = delete;
    SunabaProgram& operator=(const SunabaProgram&) = delete;
    ~SunabaProgram();

    AccessProxy operator[](int i);
    void show();
};

inline SunabaProgram::Permission SunabaProgram::get_permission(int i) const
{
    if (0 <= i && i <= 39999) return Permission::ReadWrite;
    if (50000 <= i && i <= 50009) return Permission::ReadOnly;
    if (55000 <= i && i <= 69999) return Permission::WriteOnly;
    return Permission::NoAccess;
}

inline SunabaProgram::AccessProxy::AccessProxy(int* ptr, Permission perm, int idx, SunabaProgram* self)
    : m_ptr(ptr), m_perm(perm), m_idx(idx), m_self(self) {}

inline SunabaProgram::AccessProxy::operator int() const
{
    if (m_perm != Permission::ReadWrite && m_perm != Permission::ReadOnly)
        throw std::runtime_error("read access denied");
    return *m_ptr;
}

inline SunabaProgram::AccessProxy& SunabaProgram::AccessProxy::operator=(int value)
{
    if (m_perm != Permission::ReadWrite && m_perm != Permission::WriteOnly)
        throw std::runtime_error("write access denied");
    *m_ptr = value;
    if (m_idx == 55000) m_self->trigger_sync();
    if (m_idx == 55001) m_self->m_auto_sync = (value == 0);
    return *this;
}

inline DWORD WINAPI SunabaProgram::thread_proc(LPVOID lpParam)
{
    auto* self = static_cast<SunabaProgram*>(lpParam);
    self->run_window_loop();
    return 0;
}

inline void SunabaProgram::run_window_loop()
{
    create_window();
    while (m_running && update_window())
    {
        EnterCriticalSection(&m_cs);
        if (m_need_sync || m_auto_sync)
        {
            sync_pixels();
            m_need_sync = false;
        }
        LeaveCriticalSection(&m_cs);
        Sleep(16);
    }
    destroy_window();
    DeleteCriticalSection(&m_cs);
}

inline void SunabaProgram::sync_pixels()
{
    for (int y = 0; y < 100; ++y)
        for (int x = 0; x < 100; ++x)
        {
            int c = mem[60000 + y * 100 + x];
            set_pixel(x, y, (uint8_t)(c / 10000), (uint8_t)((c / 100) % 100), (uint8_t)(c % 100));
        }
}

inline void SunabaProgram::trigger_sync()
{
    EnterCriticalSection(&m_cs);
    m_need_sync = true;
    LeaveCriticalSection(&m_cs);
}

inline SunabaProgram::AccessProxy SunabaProgram::operator[](int i)
{
    if (i < 0 || i >= MXmem)
        throw std::runtime_error("index out of range");
    return AccessProxy(&mem[i], get_permission(i), i, this);
}

inline void SunabaProgram::show()
{
    InitializeCriticalSection(&m_cs);
    m_running = true;
    m_thread = CreateThread(nullptr, 0, thread_proc, this, 0, nullptr);
}

inline SunabaProgram::~SunabaProgram()
{
    if (m_running)
    {
        m_running = false;
        WaitForSingleObject(m_thread, INFINITE);
        CloseHandle(m_thread);
    }
}

#endif