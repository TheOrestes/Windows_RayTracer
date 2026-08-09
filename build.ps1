<#
.SYNOPSIS
    One-command build for WindowsRayTracer.

.DESCRIPTION
    Locates CMake (falling back to the copy that ships with Visual Studio),
    configures the project, and builds it. All third-party dependencies are
    downloaded and built by the configure step -- nothing else to install.

.EXAMPLE
    .\build.ps1
    Configure and build Release with the Visual Studio generator.

.EXAMPLE
    .\build.ps1 -Config Debug -Run
    Build Debug and launch it from the repository root.

.EXAMPLE
    .\build.ps1 -Fresh
    Throw away the build tree (including downloaded dependencies) and start over.
#>
[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string] $Config = 'Release',

    [ValidateSet('vs2022', 'ninja')]
    [string] $Preset = 'vs2022',

    # Delete the build tree before configuring.
    [switch] $Fresh,

    # Launch the executable after a successful build.
    [switch] $Run
)

$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot

function Find-CMake {
    $onPath = Get-Command cmake -ErrorAction SilentlyContinue
    if ($onPath) { return $onPath.Source }

    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path $vswhere) {
        $vs = & $vswhere -latest -products * -property installationPath
        if ($vs) {
            $bundled = Join-Path $vs 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
            if (Test-Path $bundled) { return $bundled }
        }
    }

    throw 'CMake 3.24+ not found. Install it from https://cmake.org/download/, or install the "Desktop development with C++" workload in Visual Studio 2022 (it bundles CMake).'
}

$cmake = Find-CMake
Write-Host "Using CMake: $cmake" -ForegroundColor DarkGray

# The Ninja generator needs the MSVC toolchain on PATH; the VS generator does not.
if ($Preset -eq 'ninja' -and -not $env:VSINSTALLDIR) {
    throw 'The "ninja" preset must run from a Developer PowerShell for VS 2022. Use the default "vs2022" preset from a normal shell instead.'
}

$buildDir = Join-Path $root "build\$Preset"
if ($Fresh -and (Test-Path $buildDir)) {
    Write-Host "Removing $buildDir" -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildDir
}

Write-Host "`n=== Configuring ($Preset) ===" -ForegroundColor Cyan
Write-Host 'First run downloads and builds assimp, glfw, glew and marl -- expect a few minutes.' -ForegroundColor DarkGray
& $cmake --preset $Preset
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed (exit $LASTEXITCODE)." }

Write-Host "`n=== Building $Config ===" -ForegroundColor Cyan
& $cmake --build $buildDir --config $Config --parallel
if ($LASTEXITCODE -ne 0) { throw "Build failed (exit $LASTEXITCODE)." }

$exe = Join-Path $buildDir "$Config\WindowsRayTracer.exe"
if (-not (Test-Path $exe)) { throw "Build reported success but $exe is missing." }

Write-Host "`nBuilt: $exe" -ForegroundColor Green
Write-Host 'Models and shaders are opened with paths relative to the repository root, so run it from there:' -ForegroundColor DarkGray
Write-Host "  cd `"$root`"; & `"$exe`"" -ForegroundColor DarkGray

if ($Run) {
    Write-Host "`n=== Running ===" -ForegroundColor Cyan
    Push-Location $root
    try { & $exe } finally { Pop-Location }
}
