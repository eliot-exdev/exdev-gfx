cmake_minimum_required(VERSION 3.13)

project(exdevgfx-tools)

add_executable(convert_ppm_to_dat
        convert_ppm_to_dat.c
        )
target_link_libraries(convert_ppm_to_dat
        PRIVATE exdevgfx
        )

add_executable(convert_dat_to_ppm
        convert_dat_to_ppm.c
        )
target_link_libraries(convert_dat_to_ppm
        PRIVATE exdevgfx
        )

add_executable(convert_pgm_to_dat
        convert_pgm_to_dat.c
        )
target_link_libraries(convert_pgm_to_dat
        PRIVATE exdevgfx
        )

add_executable(halve_8bit
        halve_8bit.c
)
target_link_libraries(halve_8bit
        PRIVATE exdevgfx
)

add_executable(convert_to_8bit
        convert_to_8bit.c
        )
target_link_libraries(convert_to_8bit
        PRIVATE exdevgfx
        )

add_executable(convert_palette_to_pgm
        convert_palette_to_pgm.c
        )
target_link_libraries(convert_palette_to_pgm
        PRIVATE exdevgfx
        )

install(TARGETS convert_ppm_to_dat convert_dat_to_ppm convert_pgm_to_dat convert_to_8bit convert_palette_to_pgm
        RUNTIME DESTINATION bin
        )