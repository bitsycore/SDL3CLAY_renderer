cmake_minimum_required(VERSION 3.20)
include(FetchContent)

project(SDL3CLAY C)
set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# ============================================================================================
# MARK: SDL3
# ============================================================================================

if (NOT USE_FETCH_SDL)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
        if (APPLE OR UNIX)
            find_package(SDL3)
            if (NOT SDL3_FOUND)
                set(USE_FETCH_SDL TRUE)
            endif ()
        else ()
            if (EXISTS "${CMAKE_SOURCE_DIR}/vendor/SDL3/mingw/cmake/SDL3Config.cmake")
                include("vendor/SDL3/mingw/cmake/SDL3Config.cmake")
            else ()
                set(USE_FETCH_SDL TRUE)
            endif ()
        endif ()
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        if (EXISTS "${CMAKE_SOURCE_DIR}/vendor/SDL3/vc/cmake/SDL3Config.cmake")
            include("vendor/SDL3/vc/cmake/SDL3Config.cmake")
        else ()
            set(USE_FETCH_SDL TRUE)
        endif ()
    else ()
        message(WARNING "Unsupported compiler: ${CMAKE_C_COMPILER_ID}")
    endif ()
endif ()

if (USE_FETCH_SDL)
    message(WARNING "SDL3 not found, falling back to FetchContent.")
    FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.2.4
    )
    FetchContent_MakeAvailable(SDL3)
endif ()

# ============================================================================================
# MARK: SDL3_image
# ============================================================================================

if (NOT USE_FETCH_IMAGE)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
        if (APPLE OR UNIX)
            find_package(SDL3_image)
            if (NOT SDL3_IMAGE_FOUND)
                set(USE_FETCH_IMAGE TRUE)
            endif ()
        else ()
            if (EXISTS "${CMAKE_SOURCE_DIR}/vendor/SDL3_image/mingw/cmake/SDL3_imageConfig.cmake")
                include("vendor/SDL3_image/mingw/cmake/SDL3_imageConfig.cmake")
            else ()
                set(USE_FETCH_IMAGE TRUE)
            endif ()
        endif ()
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        if (EXISTS "${CMAKE_SOURCE_DIR}/vendor/SDL3_image/vc/cmake/SDL3_imageConfig.cmake")
            include("vendor/SDL3_image/vc/cmake/SDL3_imageConfig.cmake")
        else ()
            set(USE_FETCH_IMAGE TRUE)
        endif ()
    else ()
        message(WARNING "Unsupported compiler: ${CMAKE_C_COMPILER_ID}")
    endif ()
endif ()

if (USE_FETCH_IMAGE)
    message(WARNING "SDL3_image not found, falling back to FetchContent.")
    FetchContent_Declare(
            SDL3_image
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
            GIT_TAG release-3.2.0
    )
    FetchContent_MakeAvailable(SDL3_image)

endif ()

# ============================================================================================
# MARK: SDL3_ttf
# ============================================================================================

if (NOT USE_FETCH_TTF)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
        if (APPLE OR UNIX)
            find_package(SDL3_ttf)
            if (NOT SDL3_TTF_FOUND)
                set(USE_FETCH_TTF TRUE)
            endif ()
        else ()
            if (EXISTS "${CMAKE_SOURCE_DIR}/vendor/SDL3_ttf/mingw/cmake/SDL3_ttfConfig.cmake")
                include("vendor/SDL3_ttf/mingw/cmake/SDL3_ttfConfig.cmake")
            else ()
                set(USE_FETCH_TTF TRUE)
            endif ()
        endif ()
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        if (EXISTS "${CMAKE_SOURCE_DIR}/vendor/SDL3_ttf/vc/cmake/SDL3_ttfConfig.cmake")
            include("vendor/SDL3_ttf/vc/cmake/SDL3_ttfConfig.cmake")
        else ()
            set(USE_FETCH_TTF TRUE)
        endif ()
    else ()
        message(WARNING "Unsupported compiler: ${CMAKE_C_COMPILER_ID}")
    endif ()
endif ()

if (USE_FETCH_TTF)
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

if (MSVC)
    add_compile_options(
        /we4047
        /we4024
        /we4090
    )
else()
    add_compile_options(
        -Werror=pedantic
        -Werror=conversion
        -Werror=pointer-sign
        -Werror=incompatible-pointer-types
    )
endif()

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