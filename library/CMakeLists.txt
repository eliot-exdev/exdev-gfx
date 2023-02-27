cmake_minimum_required(VERSION 3.18)

project(exdevgfx)
find_package(X11 REQUIRED)

add_subdirectory(easing)


if (MORPHOS)
    add_library(${PROJECT_NAME} STATIC
            src/args.c
            src/color.c
            src/events.c
            src/font.c
            src/framebuffer_8bit.c
            src/framebuffer.c
            src/framebuffer_rgba.c
            src/heightmap.c
            src/helper.c
            src/julia.c
            src/matrix.c
            src/palette.c
            src/sw_renderer_8bit.c
            src/sw_renderer.c
            src/vertex2d.c
            src/vertex3d.c
            src/voxelspace.c
            src_amiga/exdev_base_amiga.c
            src_amiga/helper_amiga.c
            src_amiga/window_amiga.c
            )
    target_include_directories(${PROJECT_NAME}
            PUBLIC include
            )
    target_link_libraries(${PROJECT_NAME}
            PUBLIC m
            )
    target_compile_options(${PROJECT_NAME}
            PUBLIC -Wno-format
            )
else ()
    add_library(${PROJECT_NAME} STATIC
            src/args.c
            src/color.c
            src/events.c
            src/font.c
            src/framebuffer_8bit.c
            src/framebuffer.c
            src/framebuffer_rgba.c
            src/heightmap.c
            src/helper.c
            src/julia.c
            src/matrix.c
            src/palette.c
            src/sw_renderer_8bit.c
            src/sw_renderer.c
            src/vertex2d.c
            src/vertex3d.c
            src/voxelspace.c
            src_linux/exdev_base_linux.c
            src_linux/helper_linux.c
            src_linux/window_linux.c
            )
    target_include_directories(${PROJECT_NAME}
            PUBLIC include
            PUBLIC ${X11_INCLUDE_DIR}
            )
    target_link_libraries(${PROJECT_NAME}
            PUBLIC ${X11_LIBRARIES}
            PUBLIC m
            )
    target_compile_options(${PROJECT_NAME}
            PUBLIC -Wno-c2x-extensions
            )
endif ()



install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

install(DIRECTORY include
        DESTINATION include
        )
