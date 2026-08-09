cmake_minimum_required(VERSION 3.24)

# Every third-party library is downloaded and built at configure time -- nothing
# has to be installed or copied into external/ by hand. See BUILDING.md.

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# The app and every fetched dependency must agree on the CRT, because the
# prebuilt Open Image Denoise binaries are built against the dynamic one.
cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Some pinned dependencies still declare a cmake_minimum_required below 3.5,
# which CMake 4 rejects outright. This is the documented escape hatch.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(WindowsRayTracer LANGUAGES C CXX)

if(NOT WIN32)
    message(FATAL_ERROR
        "WindowsRayTracer is Windows-only: it compiles a Win32 resource script "
        "and links against a Windows-only Open Image Denoise binary drop.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_property(RT_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT RT_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

include(RayTracerSources)
include(RayTracerDependencies)

add_executable(WindowsRayTracer ${RT_SOURCES} ${RT_HEADERS} ${RT_RESOURCES})

# Headers are included both bare ("Application.h") and relative
# ("../RayTracer/Scene.h") depending on the commit, so expose all three roots.
target_include_directories(WindowsRayTracer PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/Main"
    "${CMAKE_SOURCE_DIR}/RayTracer")

target_link_libraries(WindowsRayTracer PRIVATE ${RT_DEPENDENCY_TARGETS})

if(MSVC)
    target_compile_options(WindowsRayTracer PRIVATE /MP)
endif()

# The original .vcxproj built with CharacterSet=Unicode on every configuration,
# and the code relies on it: it passes L"..." literals straight to MessageBox.
target_compile_definitions(WindowsRayTracer PRIVATE
    UNICODE
    _UNICODE
    _CRT_SECURE_NO_WARNINGS)

# master/IntelDenoiser ship a WinMain entry point; the OpenGL branch uses main().
if(RT_IS_WIN32_APP)
    set_target_properties(WindowsRayTracer PROPERTIES WIN32_EXECUTABLE TRUE)
    target_compile_definitions(WindowsRayTracer PRIVATE _WINDOWS)
else()
    target_compile_definitions(WindowsRayTracer PRIVATE _CONSOLE)
endif()

# Scene files, models and GLSL shaders are opened with paths relative to the
# working directory ("models/Cornell.fbx", "Main/vsScreenQuad.glsl"), so the
# app must run from the repository root.
set_target_properties(WindowsRayTracer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT WindowsRayTracer)

# Put the shared libraries the app needs next to the executable.
add_custom_command(TARGET WindowsRayTracer POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_RUNTIME_DLLS:WindowsRayTracer>" "$<TARGET_FILE_DIR:WindowsRayTracer>"
    COMMAND_EXPAND_LISTS
    VERBATIM)

# Mirror the source tree in the Visual Studio solution explorer.
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${RT_SOURCES} ${RT_HEADERS} ${RT_RESOURCES})

if(RT_IS_WIN32_APP)
    set(_rt_entry "WinMain (windows subsystem)")
else()
    set(_rt_entry "main (console subsystem)")
endif()
string(REPLACE ";" "\n                  " _rt_dep_list "${RT_ENABLED_DEPENDENCIES}")

message(STATUS "")
message(STATUS "WindowsRayTracer configured")
message(STATUS "  sources found : ${RT_SOURCE_COUNT}")
message(STATUS "  entry point   : ${_rt_entry}")
message(STATUS "  dependencies  : ${_rt_dep_list}")
message(STATUS "  run from      : ${CMAKE_SOURCE_DIR}")
message(STATUS "")
