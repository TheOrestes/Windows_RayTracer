# Third-party dependencies.
#
# Everything is fetched at configure time and built from source, except Open
# Image Denoise: building OIDN needs ISPC plus a TBB tree and takes far longer
# than the renderer itself, so the official prebuilt Windows drop is downloaded
# and imported instead (that binary is a real release asset, so its hash is
# stable and pinned below).
#
# To bump a dependency, change the pin here and nothing else.

include(FetchContent)

set(RT_GLM_TAG      "0.9.9.8")
set(RT_STB_COMMIT   "f0569113c93ad095470c54bf34a17b36646bbbb5")
set(RT_GLFW_TAG     "3.4")
set(RT_GLEW_TAG     "glew-cmake-2.2.0")
set(RT_ASSIMP_TAG   "v5.4.3")
set(RT_MARL_COMMIT  "b8406ab0a82524170ead6dd6c371b579bbc28b3e")
set(RT_OIDN_VERSION "0.8.2")
set(RT_OIDN_SHA256  "d6a60429197f6505a8b1519b977bb0aec5fd3d8a435a24429991c9928ba78f8b")

# Dependency source trees are noisy; keep them out of IDE target lists.
set(FETCHCONTENT_QUIET OFF CACHE BOOL "Show dependency download progress")
set(CMAKE_FOLDER "external")

set(RT_DEPENDENCY_TARGETS "")
set(RT_ENABLED_DEPENDENCIES "")

macro(_rt_add_dependency label)
    list(APPEND RT_ENABLED_DEPENDENCIES "${label}")
    list(APPEND RT_DEPENDENCY_TARGETS ${ARGN})
endmacro()

# ---------------------------------------------------------------------------
# Header-only libraries.
#
# SOURCE_SUBDIR points at a directory that does not exist on purpose: it makes
# FetchContent download and unpack the archive without configuring the upstream
# CMake project, which for these two would only add noise (and, for glm 0.9.9.x,
# a cmake_minimum_required too old for current CMake).
# ---------------------------------------------------------------------------

if(RT_USE_GLM)
    FetchContent_Declare(glm
        URL "https://github.com/g-truc/glm/archive/refs/tags/${RT_GLM_TAG}.tar.gz"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        SOURCE_SUBDIR "headers-only-do-not-configure")
    FetchContent_MakeAvailable(glm)

    add_library(rt_glm INTERFACE)
    # Sources say #include "glm/glm.hpp", so the archive root is the include dir.
    target_include_directories(rt_glm SYSTEM INTERFACE "${glm_SOURCE_DIR}")
    _rt_add_dependency("glm ${RT_GLM_TAG}" rt_glm)
endif()

if(RT_USE_STB)
    FetchContent_Declare(stb
        URL "https://github.com/nothings/stb/archive/${RT_STB_COMMIT}.tar.gz"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        SOURCE_SUBDIR "headers-only-do-not-configure")
    FetchContent_MakeAvailable(stb)

    add_library(rt_stb INTERFACE)
    # Sources say #include "stb_image_write.h" -- unprefixed.
    target_include_directories(rt_stb SYSTEM INTERFACE "${stb_SOURCE_DIR}")
    _rt_add_dependency("stb ${RT_STB_COMMIT}" rt_stb)
endif()

# ---------------------------------------------------------------------------
# Compiled libraries. All are built static so the only DLLs the app needs at
# runtime are the three that ship inside the Open Image Denoise drop.
# ---------------------------------------------------------------------------

if(RT_USE_GLFW)
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS    OFF)
    set(GLFW_BUILD_DOCS     OFF)
    set(GLFW_INSTALL        OFF)
    FetchContent_Declare(glfw
        URL "https://github.com/glfw/glfw/archive/refs/tags/${RT_GLFW_TAG}.tar.gz"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(glfw)
    _rt_add_dependency("glfw ${RT_GLFW_TAG}" glfw)
endif()

if(RT_USE_GLEW)
    # Perlmint's fork of GLEW, used because upstream GLEW's CMake lives in a
    # subdirectory and expects a separate code-generation step first.
    set(glew-cmake_BUILD_SHARED OFF)
    set(glew-cmake_BUILD_STATIC ON)
    set(ONLY_LIBS               ON)
    set(USE_GLU                 OFF)
    FetchContent_Declare(glew
        URL "https://github.com/Perlmint/glew-cmake/archive/refs/tags/${RT_GLEW_TAG}.tar.gz"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(glew)
    # libglew_static already propagates its include dir and GLEW_STATIC.
    _rt_add_dependency("glew ${RT_GLEW_TAG} (static)" libglew_static)
endif()

if(RT_USE_ASSIMP)
    set(ASSIMP_BUILD_TESTS         OFF)
    set(ASSIMP_BUILD_ASSIMP_TOOLS  OFF)
    set(ASSIMP_BUILD_SAMPLES       OFF)
    set(ASSIMP_INSTALL             OFF)
    set(ASSIMP_WARNINGS_AS_ERRORS  OFF)
    set(ASSIMP_BUILD_ZLIB          ON)
    set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)

    # assimp honours the global BUILD_SHARED_LIBS; force static for this
    # subdirectory only, then restore whatever the user asked for.
    set(_rt_saved_shared_libs "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS OFF)
    FetchContent_Declare(assimp
        URL "https://github.com/assimp/assimp/archive/refs/tags/${RT_ASSIMP_TAG}.tar.gz"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(assimp)
    set(BUILD_SHARED_LIBS "${_rt_saved_shared_libs}")

    _rt_add_dependency("assimp ${RT_ASSIMP_TAG} (static)" assimp::assimp)
endif()

if(RT_USE_MARL)
    set(MARL_BUILD_SHARED     OFF)
    set(MARL_BUILD_TESTS      OFF)
    set(MARL_BUILD_EXAMPLES   OFF)
    set(MARL_BUILD_BENCHMARKS OFF)
    set(MARL_INSTALL          OFF)
    FetchContent_Declare(marl
        URL "https://github.com/google/marl/archive/${RT_MARL_COMMIT}.tar.gz"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(marl)
    _rt_add_dependency("marl ${RT_MARL_COMMIT}" marl)
endif()

# ---------------------------------------------------------------------------
# Open Image Denoise: prebuilt binary drop wrapped in imported targets.
# ---------------------------------------------------------------------------

if(RT_USE_OIDN)
    set(_rt_oidn_dir "oidn-${RT_OIDN_VERSION}.x64.vc14.windows")
    FetchContent_Declare(oidn
        URL "https://github.com/OpenImageDenoise/oidn/releases/download/v${RT_OIDN_VERSION}/${_rt_oidn_dir}.zip"
        URL_HASH "SHA256=${RT_OIDN_SHA256}"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        SOURCE_SUBDIR "prebuilt-do-not-configure")
    FetchContent_MakeAvailable(oidn)

    add_library(rt_oidn INTERFACE)
    # The drop ships TBB alongside OIDN; both are needed at link and run time.
    foreach(_lib OpenImageDenoise tbb tbbmalloc)
        set(_rt_implib "${oidn_SOURCE_DIR}/lib/${_lib}.lib")
        set(_rt_dll    "${oidn_SOURCE_DIR}/bin/${_lib}.dll")
        if(NOT EXISTS "${_rt_implib}" OR NOT EXISTS "${_rt_dll}")
            message(FATAL_ERROR "Open Image Denoise drop is missing ${_lib}: unpacked to ${oidn_SOURCE_DIR}")
        endif()
        add_library(rt_oidn_${_lib} SHARED IMPORTED)
        set_target_properties(rt_oidn_${_lib} PROPERTIES
            IMPORTED_LOCATION "${_rt_dll}"
            IMPORTED_IMPLIB   "${_rt_implib}")
        target_link_libraries(rt_oidn INTERFACE rt_oidn_${_lib})
    endforeach()

    # Sources say #include "OpenImageDenoise/oidn.hpp".
    target_include_directories(rt_oidn SYSTEM INTERFACE "${oidn_SOURCE_DIR}/include")

    # This drop is release-only and built against the dynamic release CRT. That
    # is fine even for Debug builds -- the OIDN API hands out opaque handles, so
    # no CRT object crosses the DLL boundary.
    _rt_add_dependency("Open Image Denoise ${RT_OIDN_VERSION} (prebuilt)" rt_oidn)
endif()

if(RT_USE_OPENGL)
    find_package(OpenGL REQUIRED)
    _rt_add_dependency("OpenGL (system)" OpenGL::GL)
endif()

unset(CMAKE_FOLDER)
