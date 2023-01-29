cmake_minimum_required(VERSION 3.18)

project(exdevgfx)
find_package(X11 REQUIRED)

add_subdirectory(easing)

file(GLOB ${PROJECT_NAME}_HEADERS
        include
        )

file(GLOB ${PROJECT_NAME}_SOURCES
        src/*.c
        src_linux/*.c
        )

add_library(${PROJECT_NAME} STATIC
        ${${PROJECT_NAME}_HEADERS}
        ${${PROJECT_NAME}_SOURCES}
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

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

install(DIRECTORY include
        DESTINATION include
        )
