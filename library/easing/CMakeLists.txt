cmake_minimum_required(VERSION 3.18)


add_library(easing STATIC
        src/Back.c
        src/Bounce.c
        src/Circ.c
        src/Cubic.c
        src/Elastic.c
        src/Expo.c
        src/Linear.c
        src/Quad.c
        src/Quart.c
        src/Quint.c
        src/Sine.c
        )
target_include_directories(easing
        PUBLIC include
        )
target_compile_options(easing
        PRIVATE -Wno-unsequenced -Wno-sequence-point)