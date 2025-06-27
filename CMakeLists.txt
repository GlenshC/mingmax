cmake_minimum_required(VERSION 3.14)
project(testapp C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===============================
# 🧩 GLFW: Use prebuilt if available, else fetch
# ===============================
set(GLFW_ROOT_DIR "${CMAKE_SOURCE_DIR}/vendor/GLFW")
set(GLFW_INCLUDE_DIR "${GLFW_ROOT_DIR}/include")
set(GLFW_LIB_DIR "")

# Detect platform + compiler to choose right lib path
if (MSVC)
    set(GLFW_LIB_DIR "${GLFW_ROOT_DIR}/lib-vc2022")
else()
    set(GLFW_LIB_DIR "${GLFW_ROOT_DIR}/lib-mingw-w64")
endif()

# Look for prebuilt static or dynamic lib
find_library(GLFW_LIB glfw3 HINTS "${GLFW_LIB_DIR}")

if (GLFW_LIB AND EXISTS "${GLFW_INCLUDE_DIR}/GLFW/glfw3.h")
    message(STATUS "Using local prebuilt GLFW: ${GLFW_LIB}")
    add_library(glfw STATIC IMPORTED)
    set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${GLFW_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIR}"
    )
else()
    message(STATUS "Prebuilt GLFW not found — using FetchContent")

    include(FetchContent)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.8
    )
    FetchContent_MakeAvailable(glfw)
endif()
# Find OpenGL
find_package(OpenGL REQUIRED)

# ===============================
# ✨ Compiler flags
# ===============================
# Apply flags for all configurations
foreach(lang C)
    if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        add_compile_options(
            "$<$<COMPILE_LANGUAGE:${lang}>:-Wall>"
            "$<$<COMPILE_LANGUAGE:${lang}>:-Wextra>"
            "$<$<COMPILE_LANGUAGE:${lang}>:-Werror>"
        )
        set(CMAKE_C_FLAGS_DEBUG   "-g -O0")
        set(CMAKE_C_FLAGS_RELEASE "-O3")
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/W4 /WX)
        set(CMAKE_C_FLAGS_DEBUG   "/Od /Zi")
        set(CMAKE_C_FLAGS_RELEASE "/O2")
    endif()
endforeach()

# ===============================
# 🔧 Add a global macro here if needed
# ===============================
# add_compile_definitions(MY_MACRO=1)

# ===============================
# 🧱 Libraries
# ===============================
# Build glad from gl.c manually
add_library(glad STATIC
    vendor/glad/src/gl.c
)

target_include_directories(glad PUBLIC
    vendor/glad/include
)

add_library(cglm STATIC
    vendor/cglm/src/euler.c
    vendor/cglm/src/affine.c
    vendor/cglm/src/io.c
    vendor/cglm/src/quat.c
    vendor/cglm/src/cam.c
    vendor/cglm/src/vec2.c
    vendor/cglm/src/ivec2.c
    vendor/cglm/src/vec3.c
    vendor/cglm/src/ivec3.c
    vendor/cglm/src/vec4.c
    vendor/cglm/src/ivec4.c
    vendor/cglm/src/mat2.c
    vendor/cglm/src/mat2x3.c
    vendor/cglm/src/mat2x4.c
    vendor/cglm/src/mat3.c
    vendor/cglm/src/mat3x2.c
    vendor/cglm/src/mat3x4.c
    vendor/cglm/src/mat4.c
    vendor/cglm/src/mat4x2.c
    vendor/cglm/src/mat4x3.c
    vendor/cglm/src/plane.c
    vendor/cglm/src/noise.c
    vendor/cglm/src/frustum.c
    vendor/cglm/src/box.c
    vendor/cglm/src/aabb2d.c
    vendor/cglm/src/project.c
    vendor/cglm/src/sphere.c
    vendor/cglm/src/ease.c
    vendor/cglm/src/curve.c
    vendor/cglm/src/bezier.c
    vendor/cglm/src/ray.c
    vendor/cglm/src/affine2d.c
    vendor/cglm/src/clipspace/ortho_lh_no.c
    vendor/cglm/src/clipspace/ortho_lh_zo.c
    vendor/cglm/src/clipspace/ortho_rh_no.c
    vendor/cglm/src/clipspace/ortho_rh_zo.c
    vendor/cglm/src/clipspace/persp_lh_no.c
    vendor/cglm/src/clipspace/persp_lh_zo.c
    vendor/cglm/src/clipspace/persp_rh_no.c
    vendor/cglm/src/clipspace/persp_rh_zo.c
    vendor/cglm/src/clipspace/view_lh_no.c
    vendor/cglm/src/clipspace/view_lh_zo.c
    vendor/cglm/src/clipspace/view_rh_no.c
    vendor/cglm/src/clipspace/view_rh_zo.c
    vendor/cglm/src/clipspace/project_no.c
    vendor/cglm/src/clipspace/project_zo.c
)

target_include_directories(cglm PUBLIC
    vendor/cglm/include
)

# ===============================
# 🚀 Your executable
# ===============================
add_executable(
    ${PROJECT_NAME}
    src/assets/atlas.c
    src/core/app.c
    src/core/platform.c
    src/core/clock.c
    src/game/main.c
    src/game/game.c
    src/renderer/renderer.c
    src/renderer/r_batch.c
    src/renderer/r_shader.c
    src/renderer/r_texture.c
    src/util/memory.c
    src/util/string.c
# ADD MORE FILES HERE AS YOU GO
)

# Include glad headers
target_include_directories(${PROJECT_NAME} PRIVATE
    src
    vendor/glad/include
    vendor/stb_image/include
    vendor/cglm/include
)

# Link everything
target_link_libraries(${PROJECT_NAME}
    cglm
    glad
    glfw
    OpenGL::GL
)

