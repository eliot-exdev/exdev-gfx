cmake_minimum_required(VERSION 3.13)

#
# Looks c2p.library includes.
#
# You can get it from aminet.net: https://aminet.net/dev/misc/c2plib.lha
#
# Sets:
#  AMIGA_C2P_INCLUDE_DIR
#  AMIGA_C2P_LIBRARY
#  AMIGA_C2P_FOUND
#
# Searches in:
#  AMIGA_C2P_DIR
#

find_path(AMIGA_C2P_INCLUDE_DIR c2p.h
        HINTS ${AMIGA_C2P_DIR}
        HINTS ENV AMIGA_C2P_DIR
        NO_DEFAULT_PATH
        PATH_SUFFIXES sdk/C
)
message(STATUS "AMIGA_C2P_INCLUDE_DIR=${AMIGA_C2P_INCLUDE_DIR}")

find_file(AMIGA_C2P_LIBRARY c2p.lib
        HINTS ${AMIGA_C2P_DIR}
        HINTS ENV AMIGA_C2P_DIR
        NO_DEFAULT_PATH
        PATH_SUFFIXES sdk)
message(STATUS "AMIGA_C2P_LIBRARY=${AMIGA_C2P_LIBRARY}")

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(AMIGA_C2P REQUIRED_VARS AMIGA_C2P_INCLUDE_DIR AMIGA_C2P_LIBRARY)

mark_as_advanced(AMIGA_C2P_INCLUDE_DIR)
