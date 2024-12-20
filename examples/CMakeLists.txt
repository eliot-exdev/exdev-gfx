cmake_minimum_required(VERSION 3.13)

project(exdevgfx-examples)

add_executable(test_vector3d
        test_vector3d.c
        )
target_link_libraries(test_vector3d
        exdevgfx
        )

add_executable(test_matrix
        test_matrix.c
        )
target_link_libraries(test_matrix
        exdevgfx
        )

add_executable(test_graphics2d
        test_graphics2d.c
        )
target_link_libraries(test_graphics2d
        exdevgfx
        )

add_executable(test_window
        test_window.c
        )
target_link_libraries(test_window
        exdevgfx
        )

add_executable(test_logger
        test_logger.c
        )
target_link_libraries(test_logger
        exdevgfx
        )

add_executable(julia
        julia_main.c
        )
target_link_libraries(julia
        exdevgfx
        )
StripTargetExdev(julia)

add_executable(voxelspace
        voxelspace_main.c
        )
target_link_libraries(voxelspace
        exdevgfx
        )
StripTargetExdev(voxelspace)
if(AMIGAOS)
        target_compile_definitions(voxelspace PRIVATE LOW_RESOLUTION)
        set_target_properties(voxelspace PROPERTIES OUTPUT_NAME "voxelspace_amiga_060")
elseif (MORPHOS)
        set_target_properties(voxelspace PROPERTIES OUTPUT_NAME "voxelspace_mos_ppc")
endif ()

add_executable(test_3d
        test_3d.c
        )
target_link_libraries(test_3d
        exdevgfx
        )

add_executable(render_palette
        render_palette.c
        )
target_link_libraries(render_palette
        exdevgfx
        )

add_executable(test_palette
        test_palette.c
        )
target_link_libraries(test_palette
        exdevgfx
        )

add_executable(test_flip_vertical
        test_flip_vertical.c
)
target_link_libraries(test_flip_vertical
        exdevgfx
)

add_executable(test_sprite
        test_sprite.c
        )
target_link_libraries(test_sprite
        exdevgfx
        easing
        )

install(TARGETS julia voxelspace
        RUNTIME DESTINATION bin
        )
