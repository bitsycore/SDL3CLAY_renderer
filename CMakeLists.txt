cmake_minimum_required(VERSION 3.23)
include(FetchContent)

project(Test C)
set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# ============================================================================================
# MARK: SDL3
# ============================================================================================

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.4
)
FetchContent_MakeAvailable(SDL3)

# ============================================================================================
# MARK: SDL3_ttf
# ============================================================================================

# HOW TO DO THAT ?

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    if(APPLE OR UNIX)
        find_package(SDL3_ttf REQUIRED)
    else()
        include("vendor/SDL3_ttf/mingw/cmake/SDL3_ttfConfig.cmake")
    endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    include("vendor/SDL3_ttf/vc/cmake/SDL3_ttfConfig.cmake")
else()
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_C_COMPILER_ID}")
endif()

# ============================================================================================
# MARK: Executable
# ============================================================================================

add_executable(Test
        src/main.c
        src/renderer/clay_renderer_SDL3.c
        src/ui/components.c
        src/ui/components.h
        src/ui/colors.c
        src/ui/colors.h
)

target_link_libraries(Test PRIVATE SDL3-shared SDL3_ttf::SDL3_ttf-shared)

# ============================================================================================
# MARK: Post Build
# ============================================================================================

# Copy SDL2.dll to runtime folder
add_custom_command(TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3-shared>
        $<TARGET_FILE_DIR:Test>
)

# Copy SDL2_ttf.dll to runtime folder
add_custom_command(TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE_DIR:Test>
)

# Move assets folder to runtime folder
add_custom_command(TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Test>/assets"
)