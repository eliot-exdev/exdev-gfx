cmake_minimum_required(VERSION 3.18)

project(exdevgfx-base)

add_subdirectory(library)
add_subdirectory(examples)
add_subdirectory(tools)

install(DIRECTORY assets
        DESTINATION share/exdev
        )
