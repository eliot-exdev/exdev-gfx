cmake_minimum_required(VERSION 3.18)

project(exdevgfx-base)

set(CMAKE_C_STANDARD 99)
add_compile_options(-Wall -Wextra -Werror)

add_subdirectory(library)
add_subdirectory(examples)
add_subdirectory(tools)

install(DIRECTORY assets
        DESTINATION share/exdev
        )
