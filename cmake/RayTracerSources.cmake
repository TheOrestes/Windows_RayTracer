# Source discovery.
#
# The file layout is not stable across the history of this repository: master
# and IntelDenoiser keep the application at the repository root, while OpenGL
# moved it into Main/. Hard-coding a file list would pin the build system to one
# commit, so sources are globbed instead. CONFIGURE_DEPENDS makes the generator
# re-run the glob on every build, so switching branch and pressing build is
# enough -- no manual re-configure.

set(_rt_source_dirs
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/Main"
    "${CMAKE_SOURCE_DIR}/RayTracer")

set(RT_SOURCES "")
set(RT_HEADERS "")
foreach(_dir IN LISTS _rt_source_dirs)
    if(NOT IS_DIRECTORY "${_dir}")
        continue()
    endif()
    file(GLOB _rt_cpp CONFIGURE_DEPENDS "${_dir}/*.cpp" "${_dir}/*.c")
    file(GLOB _rt_hdr CONFIGURE_DEPENDS "${_dir}/*.h" "${_dir}/*.hpp" "${_dir}/*.inl")
    list(APPEND RT_SOURCES ${_rt_cpp})
    list(APPEND RT_HEADERS ${_rt_hdr})
endforeach()

# A glob compiles everything present, but some commits leave a file in the tree
# after dropping it from the build. At 6064703 the old GDI entry point
# WindowsRayTracer.cpp still sits next to the new Main/Main.cpp, and the
# .vcxproj of the day compiled only the latter; globbing both defines Trace,
# SaveImage and TraceColor twice and the link fails. Nothing in the tree records
# that intent, so it has to be stated:
#
#   cmake --preset vs2022 -DRT_EXCLUDE_SOURCES="WindowsRayTracer.cpp;RayTracer/Vector3.cpp"
#
# Paths are relative to the repository root. See BUILDING.md.
set(RT_EXCLUDE_SOURCES "" CACHE STRING
    "Repo-relative source/header paths to drop from the globs (';'-separated)")

foreach(_ex IN LISTS RT_EXCLUDE_SOURCES)
    file(TO_CMAKE_PATH "${_ex}" _ex_path)
    get_filename_component(_ex_abs "${_ex_path}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")

    if(NOT "${_ex_abs}" IN_LIST RT_SOURCES AND NOT "${_ex_abs}" IN_LIST RT_HEADERS)
        # Loud on purpose: a typo here silently builds the thing you meant to drop.
        message(WARNING "RT_EXCLUDE_SOURCES: '${_ex}' matched no globbed file")
    endif()

    list(REMOVE_ITEM RT_SOURCES "${_ex_abs}")
    list(REMOVE_ITEM RT_HEADERS "${_ex_abs}")
endforeach()

list(LENGTH RT_SOURCES RT_SOURCE_COUNT)
if(RT_SOURCE_COUNT EQUAL 0)
    message(FATAL_ERROR
        "No .cpp files found under ${CMAKE_SOURCE_DIR}, Main/ or RayTracer/. "
        "Is this the WindowsRayTracer repository root?")
endif()

file(GLOB RT_RESOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*.rc")
if(RT_RESOURCES)
    enable_language(RC)
endif()

# ---------------------------------------------------------------------------
# Which third-party libraries does *this* checkout actually use?
#
# The branches differ: master needs assimp/glm/stb/OIDN, OpenGL adds
# GLFW, GLEW and marl. Detecting from the sources keeps one build system
# correct for all of them, instead of building libraries nobody includes.
# ---------------------------------------------------------------------------

set(_rt_all_files ${RT_SOURCES} ${RT_HEADERS})

function(_rt_sources_mention out_var regex)
    foreach(_f IN LISTS _rt_all_files)
        # A few files in this tree are UTF-16; file(STRINGS) picks the encoding
        # up from their byte-order mark on its own.
        file(STRINGS "${_f}" _hits REGEX "${regex}")
        if(_hits)
            set(${out_var} ON PARENT_SCOPE)
            return()
        endif()
    endforeach()
    set(${out_var} OFF PARENT_SCOPE)
endfunction()

# Includes are written with both slash flavours ("GLFW\\glfw3.h", "glm/glm.hpp").
_rt_sources_mention(_rt_detected_GLM    "glm[/\\\\]")
_rt_sources_mention(_rt_detected_STB    "stb_image")
_rt_sources_mention(_rt_detected_ASSIMP "assimp[/\\\\]")
_rt_sources_mention(_rt_detected_OIDN   "OpenImageDenoise[/\\\\]")
_rt_sources_mention(_rt_detected_GLFW   "GLFW[/\\\\]glfw")
_rt_sources_mention(_rt_detected_GLEW   "GL[/\\\\]glew")
_rt_sources_mention(_rt_detected_MARL   "marl[/\\\\]")

_rt_sources_mention(RT_IS_WIN32_APP "WinMain")

# AUTO is deliberately what gets cached, so detection re-runs on every configure
# and a branch switch cannot leave a stale ON/OFF behind.
set(_rt_tristate_help "ON, OFF, or AUTO to follow what the sources include")
foreach(_dep GLM STB ASSIMP OIDN GLFW GLEW MARL)
    set(RT_WITH_${_dep} "AUTO" CACHE STRING "${_dep}: ${_rt_tristate_help}")
    set_property(CACHE RT_WITH_${_dep} PROPERTY STRINGS AUTO ON OFF)

    if(RT_WITH_${_dep} STREQUAL "AUTO")
        set(RT_USE_${_dep} ${_rt_detected_${_dep}})
    else()
        set(RT_USE_${_dep} ${RT_WITH_${_dep}})
    endif()
endforeach()

# GLEW and GLFW both need the system OpenGL library.
if(RT_USE_GLEW OR RT_USE_GLFW)
    set(RT_USE_OPENGL ON)
else()
    set(RT_USE_OPENGL OFF)
endif()
