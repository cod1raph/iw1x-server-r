cmake_minimum_required(VERSION 3.10)

project(iw1x-server)

# C++17 is required for std::filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SEPARATOR "---------------------")

# -m32:     32-bit
# -fpic:    Position-independent code
# -Wall:    Enable all warnings
# -fvisibility=hidden: Hide symbols
set(CMAKE_CXX_FLAGS "-m32 -fpic -Wall -fvisibility=hidden")
set(CMAKE_SHARED_LINKER_FLAGS "-m32")
set(CMAKE_LIBRARY_PATH "/lib32" "/usr/lib32" "/usr/lib/i386-linux-gnu" ${CMAKE_LIBRARY_PATH})

# Options
option(DEBUG_BUILD "Include debug info, no optimization" OFF)
option(ENABLE_UNSAFE "Enable risky features (fread, fwrite...)" OFF)

message(STATUS "${SEPARATOR}")
message(STATUS "Debug: ${DEBUG_BUILD}")
message(STATUS "Risky features: ${ENABLE_UNSAFE}")
message(STATUS "${SEPARATOR}")

# -g:       Include debug info
# -ggdb:    Debug info for gdb
# -O2:      Moderate optimization
if(DEBUG_BUILD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb")
else()
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

if(ENABLE_UNSAFE)
    add_definitions(-DENABLE_UNSAFE=1)
else()
    add_definitions(-DENABLE_UNSAFE=0)
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