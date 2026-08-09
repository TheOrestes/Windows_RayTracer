# Windows RayTracer
Peter Shierly's ray tracer with GDI 
Working version for both Single threaded & multi-threaded version using SetPixel()
Added Intel's Denoiser Support (https://openimagedenoise.github.io/)

![Render Output](https://i.ibb.co/1fxjSBL/Output1.jpg)
![Render Output](https://i.ibb.co/6WPVMF3/Output2.jpg)
![Render Output](https://i.ibb.co/m5vvNBB/Output3.jpg)

## Building

Requires Windows x64 and Visual Studio 2022 with the *Desktop development with C++* workload. From the repository root:

```powershell
.\build.ps1
```

Every dependency (glm, stb, assimp, Intel Open Image Denoise) is downloaded, built and linked by the configure step — nothing to install by hand. See [BUILDING.md](BUILDING.md) for options, dependency pins and troubleshooting.

Asset Credits:
 * Barbarian : https://assetstore.unity.com/packages/3d/characters/humanoids/barbarian-warrior-75519
 * Car : https://assetstore.unity.com/packages/3d/vehicles/land/retro-cartoon-cars-cicada-96158

## License

Source code is [MIT](LICENSE) licensed — use it for anything, just keep the copyright notice.

The models and textures under `models/` are **not** covered by that license; they come from third parties under their own terms, credited above. Dependencies are fetched at build time rather than vendored, and each keeps its own license — see [LICENSE](LICENSE) for the list.

Built on Peter Shirley's *Ray Tracing in One Weekend* series (code released CC0 / public domain) and Kevin Suffern's *Ray Tracing from the Ground Up*.
