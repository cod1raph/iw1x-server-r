cmake_minimum_required(VERSION 3.10)

project(iw1x-server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SEPARATOR "---------------------")

# -m32:     32-bit
# -fpic:    Position-independent code
# -Wall:    Enable all warnings
# -fvisibility=hidden: Hide symbols
# -g:       Include debug info
# -ggdb:    Debug info for gdb

set(CMAKE_CXX_FLAGS "-m32 -fpic -Wall -fvisibility=hidden -g -ggdb")
set(CMAKE_SHARED_LINKER_FLAGS "-m32")
set(CMAKE_LIBRARY_PATH "/lib32" "/usr/lib32" "/usr/lib/i386-linux-gnu" ${CMAKE_LIBRARY_PATH})

# Options
option(FILE_HANDLING "Enable stdio functions (fread, fwrite...)" OFF)

message(STATUS "${SEPARATOR}")
message(STATUS "FILE_HANDLING: ${FILE_HANDLING}")
message(STATUS "${SEPARATOR}")

if(FILE_HANDLING)
    add_definitions(-DENABLE_FILE_HANDLING=1)
else()
    add_definitions(-DENABLE_FILE_HANDLING=0)
endif()

# Source files
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/hook.cpp
    ${CMAKE_SOURCE_DIR}/src/iw1x.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc_entity.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc_player.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc_weapons.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc_utils.cpp
)

add_library(iw1x SHARED ${SOURCES})
target_precompile_headers(iw1x PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)

# Output properties
set_target_properties(iw1x PROPERTIES
    OUTPUT_NAME "iw1x"
    PREFIX ""
    SUFFIX ".so"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin"
)

# Linked libraries
target_link_libraries(iw1x
    dl
    pthread
)