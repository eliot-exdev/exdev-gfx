cmake_minimum_required(VERSION 3.13)

add_library(fpmath INTERFACE)
target_include_directories(fpmath
        INTERFACE include
)

add_executable(fpmath_test
        src/fpmath_test.c
)
target_link_libraries(fpmath_test
        PRIVATE fpmath
)
target_compile_definitions(fpmath_test PRIVATE EXDEV_FP_MATH)
target_link_libraries(fpmath_test PRIVATE m)
