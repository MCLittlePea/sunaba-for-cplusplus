cmake_minimum_required(VERSION 3.15)
project(sunaba-for-cplusplus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sunaba-for-cplusplus STATIC
        pixel_render.cpp
)

target_link_libraries(sunaba-for-cplusplus PUBLIC gdi32 user32)
target_include_directories(sunaba-for-cplusplus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test test.cpp)
target_link_libraries(test sunaba-for-cplusplus)