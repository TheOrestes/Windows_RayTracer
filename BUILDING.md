# Building WindowsRayTracer

Every third-party library is downloaded, built and linked by the build system.
There is nothing to install by hand and nothing to copy into `external/`.

## Prerequisites

| | |
|---|---|
| Windows | x64 |
| Visual Studio 2022 | with the **Desktop development with C++** workload (this also supplies CMake 3.31 and Ninja) |
| Git | on `PATH` |
| Network access | for the first configure only |

CMake 3.24 or newer is required. If you have your own CMake on `PATH` it will be
used; otherwise the copy bundled with Visual Studio is found automatically.

Visual Studio 2022 is what this project is developed and tested against, and it
is what CI builds on. `CMakePresets.json` asks for the `Visual Studio 17 2022`
generator by name, so that is the version guaranteed to work. Newer Visual
Studio releases may build it fine — nothing here has verified that, and the
`vs2022` preset would need its generator changed to try.

## Build

From a normal PowerShell prompt in the repository root:

```powershell
.\build.ps1
```

That configures, downloads dependencies, and builds Release. Useful variants:

```powershell
.\build.ps1 -Config Debug        # Debug build
.\build.ps1 -Run                 # build, then launch from the repo root
.\build.ps1 -Fresh               # wipe build/ (and the fetched dependencies) first
.\build.ps1 -Preset ninja        # Ninja instead of MSBuild (Developer PowerShell only)
```

The first configure takes a few minutes, almost all of it spent compiling
assimp. Later configures reuse `build/vs2022/_deps` and take seconds.

### Plain CMake

```powershell
cmake --preset vs2022
cmake --build build/vs2022 --config Release --parallel
```

### Visual Studio

Either open the generated `build/vs2022/WindowsRayTracer.sln`, or open the
repository folder directly in Visual Studio — it reads `CMakePresets.json` and
offers the same presets. Either way `WindowsRayTracer` is the startup project
and its debugger working directory is already set to the repository root.

## Running

Models and GLSL shaders are opened through paths relative to the working
directory (`models/Cornell.fbx`, `Main/vsScreenQuad.glsl`), so the executable
must run from the repository root:

```powershell
& .\build\vs2022\Release\WindowsRayTracer.exe
```

The DLLs it needs are copied next to the executable after every build.

## Dependencies

| Library | Version | How it is obtained |
|---|---|---|
| glm | 0.9.9.8 | source archive, header-only |
| stb | pinned commit | source archive, header-only |
| GLFW | 3.4 | built from source, static |
| GLEW | 2.2.0 (`Perlmint/glew-cmake`) | built from source, static |
| assimp | 5.4.3 | built from source, static |
| marl | pinned commit | built from source, static |
| Open Image Denoise | 0.8.2 | official prebuilt x64 drop, SHA-256 pinned |
| OpenGL | system | `find_package(OpenGL)` |

All pins live at the top of `cmake/RayTracerDependencies.cmake`; that is the
only file to touch when bumping a version.

Open Image Denoise is the one dependency that is *not* built from source.
Building it needs ISPC and a separate TBB tree and takes longer than the
renderer itself, so the official release ZIP is downloaded and wrapped in
imported targets. Because that ZIP is an uploaded release asset its bytes are
stable, so its SHA-256 is pinned. The other archives are GitHub's
auto-generated tag tarballs, which are pinned by tag or commit but not by hash.

The drop is release-only and built against the dynamic release CRT. Linking it
into a Debug build is safe: the OIDN API only hands out opaque handles, so no
CRT object crosses the DLL boundary. Everything else is compiled against the
same dynamic CRT as the application (`CMAKE_MSVC_RUNTIME_LIBRARY`).

The prebuilt binaries under `external/` are left over from the old MSBuild
project and are no longer read. They can be deleted; only headers are tracked in
git there anyway, which is why a fresh clone never linked before.

## Working across branches and commits

The build is deliberately not tied to any one revision of this repository:

* **Sources are globbed**, not enumerated. `master` and `IntelDenoiser` keep the
  application at the repository root; `OpenGL` moved it into `Main/`. Both work.
  The globs use `CONFIGURE_DEPENDS`, so switching branch and pressing build is
  enough — files that appeared or disappeared are picked up without a manual
  re-configure.
* **The dependency set is detected from the sources.** `cmake/RayTracerSources.cmake`
  scans the collected files for the include patterns of each library, so a
  checkout of `master` builds glm, stb, assimp and OIDN and skips GLFW, GLEW and
  marl, while `OpenGL` gets all seven. Each can be forced with
  `-DRT_WITH_GLFW=ON` / `=OFF`; the cached value is the string `AUTO`, so
  detection re-runs on every configure and a branch switch cannot leave a stale
  setting behind.
* **The subsystem is detected too.** A checkout whose sources define `WinMain`
  is linked as a Windows-subsystem app; one with `main` is linked as a console
  app.

The configure summary reports what was detected:

```
-- WindowsRayTracer configured
--   sources found : 16
--   entry point   : WinMain (windows subsystem)
--   dependencies  : glm 0.9.9.8
                    stb f0569113c93ad095470c54bf34a17b36646bbbb5
                    assimp v5.4.3 (static)
                    Open Image Denoise 0.8.2 (prebuilt)
```

Two caveats.

**The build files have to exist in the checkout.** Checking out a commit from
before they were added gets you that commit's sources without a `CMakeLists.txt`.
Merge or cherry-pick this commit into each branch you want to build, or overlay
the build files onto a detached checkout of the old commit:

```powershell
git worktree add --detach ..\rt-<sha> <sha>
git -C ..\rt-<sha> checkout <this-commit> -- CMakeLists.txt CMakePresets.json cmake build.ps1
cd ..\rt-<sha>
.\build.ps1 -Run
```

Use `git -C <worktree>`; `git --work-tree=<path> checkout` writes into the main
checkout as well.

**A glob compiles every file present, which is not always what the commit
built.** Some revisions leave a source in the tree after dropping it from the
project. At `6064703` the old GDI entry point `WindowsRayTracer.cpp` still sits
beside the new `Main/Main.cpp`, and that commit's `.vcxproj` compiled only the
latter — globbing both defines `Trace`, `SaveImage` and `TraceColor` twice and
the link fails. Nothing in the tree records the exclusion, so name it:

```powershell
cmake --preset vs2022 -DRT_EXCLUDE_SOURCES="WindowsRayTracer.cpp;RayTracer/Vector3.cpp"
```

Paths are relative to the repository root, `;`-separated, and apply to headers
as well as sources. An entry matching nothing warns rather than failing, so a
typo is visible instead of silently building the file you meant to drop. Unlike
the `RT_WITH_*` switches this value is cached as given, so clear it when moving
to a commit that does not need it.

## Troubleshooting

**`CMake 3.24+ not found`** — install the *Desktop development with C++*
workload in the Visual Studio Installer, or CMake from cmake.org.

**Download failures behind a proxy** — set `HTTP_PROXY`/`HTTPS_PROXY` before
configuring, or drop pre-extracted sources in place and point CMake at them with
`-DFETCHCONTENT_SOURCE_DIR_ASSIMP=<path>` (one variable per dependency, name
upper-cased).

**Stale dependency state after editing the pins** — `.\build.ps1 -Fresh`, or
delete `build/vs2022/_deps`.

**`The "ninja" preset must run from a Developer PowerShell`** — the Ninja
generator needs the MSVC environment. Use the default `vs2022` preset from a
normal shell, or start *Developer PowerShell for VS 2022*.
