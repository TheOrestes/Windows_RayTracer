# OpenGL RayTracer
* Peter Shierly's ray tracer with OpenGL. 
* Working Single & multi-threaded version. 
* Added optional Marl Scheduler based task system.
* Intel's Denoiser integration. 
* Various samplers from RTFGU book. 

## Building

Requires Windows x64 and Visual Studio 2022 with the *Desktop development with C++* workload. From the repository root:

```powershell
.\build.ps1
```

Every dependency (glm, stb, GLFW, GLEW, assimp, marl, Intel Open Image Denoise) is downloaded, built and linked by the configure step — nothing to install by hand. See [BUILDING.md](BUILDING.md) for options, dependency pins and troubleshooting.

## Render Output
![Render Output1](https://pbs.twimg.com/media/DvuZOUmWsAAg9oR.jpg)
![Render Output2](https://pbs.twimg.com/media/DvuZOUFXQAAOCPm.jpg)
![Render Output3](https://pbs.twimg.com/media/DvuZOUsXgAI-RtL.jpg)

## Intel's Denoiser Integration (https://openimagedenoise.github.io/)

#### Denoiser Output1
  ![Denoiser Output 1](https://i.ibb.co/28HDF5j/ogl-rt1.jpg)

#### Denoiser Output2
  ![Denoiser Ouptut2](https://i.ibb.co/PYhJC4B/ogl-rt2.jpg)

#### Denoiser Output3
  ![Denoiser Output3](https://i.ibb.co/F3ByR43/ogl-rt3.jpg)

#### Denoiser Output4
  ![Denoiser Output4](https://i.ibb.co/P9M9Rmr/ogl-rt4.jpg)

#### Asset Credits:
  * Barbarian : https://assetstore.unity.com/packages/3d/characters/humanoids/barbarian-warrior-75519
  * Car : https://assetstore.unity.com/packages/3d/vehicles/land/retro-cartoon-cars-cicada-96158

## License

Source code is [MIT](LICENSE) licensed — use it for anything, just keep the copyright notice.

The models and textures under `models/` are **not** covered by that license; they come from third parties under their own terms, credited above. Dependencies are fetched at build time rather than vendored, and each keeps its own license — see [LICENSE](LICENSE) for the list.

Built on Peter Shirley's *Ray Tracing in One Weekend* series (code released CC0 / public domain) and Kevin Suffern's *Ray Tracing from the Ground Up*.

##### Compiled libs are no longer needed — the build system fetches and builds them. The old prebuilt drop is archived at [Libs](https://drive.google.com/drive/folders/12YA0c78YrrAOwK97uqeYieTNGlAK-te5?usp=sharing).

Asset Credits:
 * Barbarian : https://assetstore.unity.com/packages/3d/characters/humanoids/barbarian-warrior-75519
 * Car : https://assetstore.unity.com/packages/3d/vehicles/land/retro-cartoon-cars-cicada-96158

