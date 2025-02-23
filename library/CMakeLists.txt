cmake_minimum_required(VERSION 3.13)

project(exdevgfx)
find_package(X11 REQUIRED)

# add external libraries
add_subdirectory(fpmath)
add_subdirectory(easing)

set(EXDEV_ENABLE_FIX_POINT False CACHE BOOL "Enables/Disables fix point mathematics")
message(STATUS "EXDEV_ENABLE_FIX_POINT=${EXDEV_ENABLE_FIX_POINT}")

if (MORPHOS OR AMIGAOS)
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
            PUBLIC -Wno-format -Wno-int-conversion
    )
    if (EXDEV_ENABLE_FIX_POINT)
        target_compile_definitions(${PROJECT_NAME} PUBLIC EXDEV_FP_MATH)
        target_link_libraries(${PROJECT_NAME} PUBLIC fpmath)
    else ()
        target_link_libraries(${PROJECT_NAME} PUBLIC m)
    endif ()
    if (AMIGAOS)
        target_compile_definitions(${PROJECT_NAME} PRIVATE LOW_RESOLUTION)
    endif ()
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
            src_linux/window_linux_x11.c
    )
    target_include_directories(${PROJECT_NAME}
            PUBLIC include
            PUBLIC ${X11_INCLUDE_DIR}
    )
    target_link_libraries(${PROJECT_NAME}
            PUBLIC ${X11_LIBRARIES}
    )
    target_compile_options(${PROJECT_NAME}
            PUBLIC -Wno-c2x-extensions -Wno-unused-variable -fPIC -Wno-format
    )
    if (EXDEV_ENABLE_FIX_POINT)
        target_compile_definitions(${PROJECT_NAME} PUBLIC EXDEV_FP_MATH)
        target_link_libraries(${PROJECT_NAME} PUBLIC fpmath)
    else ()
        target_link_libraries(${PROJECT_NAME} PUBLIC m)
    endif ()
endif ()

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY include
        DESTINATION include
)
