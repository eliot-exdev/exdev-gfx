cmake_minimum_required(VERSION 3.13)

project(exdevgfx-base)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# some helper
function(StripTargetExdev TARGET_NAME)
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

if(MORPHOS)
    message(STATUS "Compiling for MorphOs")
elseif (AMIGAOS)
    message(STATUS "Compiling for AmigaOs")
    set(USE_AMIGA_C2P OFF CACHE BOOL "enable using own c2p routine instead of amiga system c2p")
    message("USE_AMIGA_C2P=${USE_AMIGA_C2P}")
    if(USE_AMIGA_C2P)
        find_package(AMIGA_C2P REQUIRED)
    endif ()
else()
    message(STATUS "Compiling for any Os")
endif ()

add_subdirectory(library)
add_subdirectory(examples)
add_subdirectory(tools)

install(DIRECTORY assets
        DESTINATION share/exdev
)
