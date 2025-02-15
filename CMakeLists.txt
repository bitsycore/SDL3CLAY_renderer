cmake_minimum_required(VERSION 3.20)
include(FetchContent)

project(SDL3CLAY C)
set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(COMPILER_MSVC "MSVC")
set(COMPILER_MINGW "MinGW")
set(COMPILER_GCC "GCC")
set(COMPILER_CLANG "Clang")
set(COMPILER_UNKNOWN "Unknown")

set(OS_WINDOWS "Windows")
set(OS_MACOS "macOS")
set(OS_IOS "iOS")
set(OS_LINUX "Linux")
set(OS_ANDROID "Android")
set(OS_UNKNOWN "Unknown")

# Detect OS
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(DETECTED_OS ${OS_WINDOWS})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if (CMAKE_OSX_SYSROOT MATCHES ".*iPhoneOS.*")
        set(DETECTED_OS ${OS_IOS})
    else()
        set(DETECTED_OS ${OS_MACOS})
    endif()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(DETECTED_OS ${OS_LINUX})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(DETECTED_OS ${OS_ANDROID})
else()
    set(DETECTED_OS ${OS_UNKNOWN})
endif()

message(STATUS "Detected OS: ${DETECTED_OS}")

# Detect Compiler
if (MSVC)
    set(DETECTED_COMPILER ${COMPILER_MSVC})
elseif (MINGW)
    set(DETECTED_COMPILER ${COMPILER_MINGW})
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(DETECTED_COMPILER ${COMPILER_GCC})
elseif (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(DETECTED_COMPILER ${COMPILER_CLANG})
else()
    set(DETECTED_COMPILER ${COMPILER_UNKNOWN})
endif()

message(STATUS "Detected Compiler: ${DETECTED_COMPILER}")

# ============================================================================================
# MARK: SDL3
# ============================================================================================

function(fetch_module_if_needed module_name git_repository git_tag)
    set(USE_FETCH_SDL_LOCAL FALSE)

    if (NOT USE_FETCH_SDL)
        if(DETECTED_OS STREQUAL OS_WINDOWS)
            if(DETECTED_COMPILER STREQUAL COMPILER_MSVC AND EXISTS "${CMAKE_SOURCE_DIR}/vendor/${module_name}/vc/cmake/${module_name}Config.cmake")
                include("${CMAKE_SOURCE_DIR}/vendor/${module_name}/vc/cmake/${module_name}Config.cmake")
            elseif(DETECTED_COMPILER STREQUAL COMPILER_MINGW AND EXISTS "${CMAKE_SOURCE_DIR}/vendor/${module_name}/mingw/cmake/${module_name}Config.cmake")
                include("${CMAKE_SOURCE_DIR}/vendor/${module_name}/mingw/cmake/${module_name}Config.cmake")
            else()
                set(USE_FETCH_SDL_LOCAL TRUE)
            endif()
        elseif(DETECTED_OS STREQUAL OS_MACOS)
            find_package(${module_name})
            if(NOT ${module_name}_FOUND)
                set(USE_FETCH_SDL_LOCAL TRUE)
            endif()
        elseif(DETECTED_OS STREQUAL OS_LINUX)
            find_package(${module_name})
            if (NOT ${module_name}_FOUND)
                set(USE_FETCH_SDL_LOCAL TRUE)
            endif()
        endif()
    endif()

    if (USE_FETCH_SDL_LOCAL)
        message("${module_name} not found, falling back to FetchContent.")
        FetchContent_Declare(
                ${module_name}
                GIT_REPOSITORY ${git_repository}
                GIT_TAG ${git_tag}
        )
        FetchContent_MakeAvailable(${module_name})
    endif ()
endfunction()

fetch_module_if_needed("SDL3" "https://github.com/libsdl-org/SDL.git" "release-3.2.4")
fetch_module_if_needed("SDL3_ttf" "https://github.com/libsdl-org/SDL_ttf.git" "preview-3.1.0")
fetch_module_if_needed("SDL3_image" "https://github.com/libsdl-org/SDL_image.git" "release-3.2.0")

# ============================================================================================
# MARK: Executable
# ============================================================================================

add_executable(SDL3CLAY
        src/main.c
        src/appstate.c
        src/renderer/SDL3CLAY.c
        src/common/debug.c
        src/common/hash.c
        src/common/memory_leak.c
        src/common/uuid.c
        src/ui/colors.c
        src/ui/screen_manager.c
        src/ui/screens/screen_main.c
        src/ui/screens/screen_test_1.c
        src/ui/screens/screen_test_2.c
        src/ui/screens/screen_test_3.c
        src/ui/components/component_debug_button.c
        src/ui/components/component_sidebar_item.c
        src/ui/components/component_profile.c
)

target_include_directories(SDL3CLAY PRIVATE ${CMAKE_SOURCE_DIR}/vendor/clay)

target_link_libraries(
        SDL3CLAY PRIVATE
        SDL3::SDL3-shared
        SDL3_image::SDL3_image-shared
        SDL3_ttf::SDL3_ttf-shared
)

target_compile_definitions(SDL3CLAY PRIVATE $<$<CONFIG:Debug>:ENABLE_LEAK_DETECTOR=1>)

if (DETECTED_COMPILER STREQUAL COMPILER_MSVC)
    add_compile_options(
            /we4047
            /we4024
            /we4090
    )
elseif (DETECTED_COMPILER STREQUAL COMPILER_CLANG)
    add_compile_options(
            -fblocks
    )
else ()
    add_compile_options(
            -Werror=pedantic
            -Werror=conversion
            -Werror=pointer-sign
            -Werror=incompatible-pointer-types
    )
endif ()

# ============================================================================================
# MARK: Post Build
# ============================================================================================

# Copy SDL3 shared libraries to runtime folder
add_custom_command(TARGET SDL3CLAY POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
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