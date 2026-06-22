cmake_minimum_required(VERSION 3.15)
project(sunaba-for-cplusplus)

set(CMAKE_CXX_STANDARD 11)

# ========== 你的静态库（保持不变） ==========
add_library(sunaba-for-cplusplus STATIC
        pixel_render.cpp
)

target_link_libraries(sunaba-for-cplusplus PUBLIC gdi32 user32)
target_include_directories(sunaba-for-cplusplus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ========== 新增：测试程序 ==========
add_executable(render_test test.cpp)
# 测试程序只需要链接你的库，自动继承 gdi32/user32 依赖
target_link_libraries(render_test sunaba-for-cplusplus)