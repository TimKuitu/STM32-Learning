cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME 04_I2CTest)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# ---------------------------------------------------------------------------
# User-defined configuration (preserved across STM32CubeMX regenerations)
# ---------------------------------------------------------------------------
set(USER_INCLUDE_DIRS
    Core/Inc
)

set(USER_SOURCES
    Core/Src/OLED.c
)

set(USER_COMPILE_DEFINITIONS
    # Add user defined symbols here
)

set(USER_LINK_DIRS
    # Add user defined library search paths here
)

set(USER_LINK_LIBS
    # Add user defined libraries here
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_LINK_DIRS}
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_SOURCES}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_INCLUDE_DIRS}
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_COMPILE_DEFINITIONS}
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    ${USER_LINK_LIBS}
)
