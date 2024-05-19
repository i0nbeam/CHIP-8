cmake_minimum_required(VERSION 3.5)

project(CHIP8)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

add_executable(chip8 chip8.c ${sources} ${headers})

target_link_libraries(chip8 libraylib.a gdi32 winmm opengl32)




















