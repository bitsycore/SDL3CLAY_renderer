cmake_minimum_required(VERSION 3.20)
include(FetchContent)

project(SDL3CLAY C)
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
# MARK: SDL3_image
# ============================================================================================

FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.2.0
)
FetchContent_MakeAvailable(SDL3_image)

# ============================================================================================
# MARK: SDL3_ttf
# ============================================================================================

if (NOT USE_FETCHCONTENT)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
        if (APPLE OR UNIX)
            find_package(SDL3_ttf)
            if (NOT SDL3_TTF_FOUND)
                set(USE_FETCHCONTENT TRUE)
            endif ()
        else ()
            if (EXISTS "${CMAKE_SOURCE_DIR}/vendor/SDL3_ttf/mingw/cmake/SDL3_ttfConfig.cmake")
                include("vendor/SDL3_ttf/mingw/cmake/SDL3_ttfConfig.cmake")
            else ()
                set(USE_FETCHCONTENT TRUE)
            endif ()
        endif ()
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        if (EXISTS "${CMAKE_SOURCE_DIR}/vendor/SDL3_ttf/vc/cmake/SDL3_ttfConfig.cmake")
            include("vendor/SDL3_ttf/vc/cmake/SDL3_ttfConfig.cmake")
        else ()
            set(USE_FETCHCONTENT TRUE)
        endif ()
    else ()
        message(WARNING "Unsupported compiler: ${CMAKE_C_COMPILER_ID}")
    endif ()
endif ()

if (USE_FETCHCONTENT)
    message(WARNING "SDL3_ttf not found, falling back to FetchContent.")
    FetchContent_Declare(
            SDL3_ttf
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
            GIT_TAG preview-3.1.0
    )
    FetchContent_MakeAvailable(SDL3_ttf)
endif ()

# ============================================================================================
# MARK: Executable
# ============================================================================================

add_compile_definitions()

add_executable(SDL3CLAY
        src/main.c
        src/renderer/SDL3CLAY.c
        src/ui/components/component_profile.c
        src/ui/colors.c
        src/appstate.c
        src/common/debug.c
        src/common/hash.c
        src/common/memory_leak.c
        src/ui/screen_manager.c
        src/ui/screens/screen_profile.c
        src/common/uuid.c
        src/ui/components/component_debug_button.c
        src/ui/components/component_sidebar_item.c
)

target_include_directories(SDL3CLAY PRIVATE ${CMAKE_SOURCE_DIR}/vendor/clay)

target_link_libraries(
        SDL3CLAY PRIVATE
        SDL3::SDL3-shared
        SDL3_image::SDL3_image-shared
        SDL3_ttf::SDL3_ttf-shared
)

# ============================================================================================
# MARK: Post Build
# ============================================================================================

target_compile_definitions(SDL3CLAY PRIVATE $<$<CONFIG:Debug>:ENABLE_LEAK_DETECTOR=1>)

# Copy SDL3 shared libraries to runtime folder
add_custom_command(TARGET SDL3CLAY POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3-shared>
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE:SDL3_image::SDL3_image>
        $<TARGET_FILE_DIR:SDL3CLAY>
)

# Move assets folder to runtime folder
add_custom_command(TARGET SDL3CLAY POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:SDL3CLAY>/assets"
)