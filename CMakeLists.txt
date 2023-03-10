cmake_minimum_required(VERSION 3.18)

project(exdevgfx-base)

# some helper
function(StripTarget TARGET_NAME)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        if (APPLE)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${TARGET_NAME}>
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "stripping target ${TARGET_NAME}"
                    )
        else ()
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_STRIP} --strip-debug --strip-unneeded $<TARGET_FILE:${TARGET_NAME}>
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "stripping target ${TARGET_NAME}"
                    )
        endif ()
    endif ()
endfunction()

set(CMAKE_C_STANDARD 99)
add_compile_options(-Wall -Wextra -Werror -Wno-pointer-sign)

message(STATUS "MORPHOS=${MORPHOS}")


add_subdirectory(library)
add_subdirectory(examples)
add_subdirectory(tools)

install(DIRECTORY assets
        DESTINATION share/exdev
        )
