# eduEngine
Course framework for **DA376B Game Engine Architecture**, Malmö University 2025  
Assets must be downloaded separately  
_Work in progress_

## Requirements
- A compiler that supports C++20
- Source control: [git](https://git-scm.com/)
- Build system: [CMake](https://cmake.org/)
- Editor or IDE of choice, for example -
  - Visual Studio 2022 with the _Desktop development with C++_ workload (this workload includes CMake).
  - Visual Studio Code (Cross Platform). Build and launch scripts included.
  - Xcode and others (not tested - please report if you have success with other IDE's)

### Under consideration
- Scripting: [LUA](https://www.lua.org/)

## Fetched dependencies
- Backend for window, sound, input etc: [SDL3](https://github.com/libsdl-org/SDL)
- OpenGL extension loader: [glew-cmake](https://github.com/Perlmint/glew-cmake)
- Model loader: [assimp](https://github.com/assimp/assimp) (v5.0.1 200112 - newer versions have [issues](https://github.com/assimp/assimp/issues/4620) with Mixamo models).
- User interface: [Dear ImGui](https://github.com/ocornut/imgui)
- Vector & matrix math: [glm](https://github.com/g-truc/glm)
- Image loader & writer: [stb](https://github.com/nothings/stb)
- Entity-Component-System: [EnTT](https://github.com/skypjack/entt)

### Under consideration
- Multi-channel audio mixer: [SDL_mixer](https://github.com/libsdl-org/SDL_mixer) (missing proper cmake support?)
- Advanced animation: [ozz-animation](https://guillaumeblanc.github.io/ozz-animation/)
- JSON reader & writer: [nlohmann-json](https://github.com/nlohmann/json)
- File dialog based on Dear ImGui: [ImGuiFileDialog](https://github.com/aiekick/ImGuiFileDialog)
- LUA binding: [sol2](https://github.com/ThePhD/sol2)

## Build Instructions
> **Note:** A clean build will take some time to complete since all dependencies are compiled from source.

### Windows with Visual Studio
```sh
# 1. Open PowerShell and clone the repository
git clone https://github.com/cjgribel/eduEngine.git

# 2. Navigate to the project directory
cd eduEngine

# (Optional: Verify that CMake is found)
cmake --version

# 3. Generate project
cmake -B Build
```
Now open the Visual Studio solution file, located in `eduEngine/Build`. Build and Run by pressing F5.

### macOS with VS Code
```sh
# Open a terminal and clone the repository
git clone https://github.com/cjgribel/eduEngine.git

# Navigate to the project directory
cd eduEngine

# (Optional: Verify that CMake is found)
cmake --version

# Generate projects for Debug & Release builds
cmake -B Debug -DCMAKE_BUILD_TYPE=Debug
cmake -B Release -DCMAKE_BUILD_TYPE=Release

# Open VS Code
code .
```
In VS Code, open the Run tab with `Shift+Cmd+D`, then select either `Debug Launch (macOS)` or `Release Launch (macOS)`. Press `F5` to Build and Run.

### Selecting a Generator
To select a specific generator (Visual Studio, Unix Makefile etc) use `cmake` e.g. like this,
```sh
cmake -B Debug -G "name-of-generator" -DCMAKE_BUILD_TYPE=Debug
```

where `name-of-generator` is replaced by the name of a supported generator. Use this command to list generators that are available on your system
```sh
cmake --help
```

### 🖥️ Visual Studio Code (Cross-Platform)

_Work in progress_

1. **Install Extensions:**
   - C/C++ (by Microsoft)
   <!-- - CMake Tools (optional) -->

2. **Run & Debug the Project:**
   - Open the project folder in VS Code.
   - Press `Ctrl+Shift+D` (Run tab) or `F5` to start debugging.
   - Choose from:
     - **Debug Launch (Windows/Linux/macOS)**
     - **Release Launch (Windows/Linux/macOS)**

> **Note:** No need to run build tasks manually. The debugger will automatically build the project before launching.

## Documentation

[Doxygen](https://cjgribel.github.io/eduEngine/) _Work in progress_


## Samples (assets not part of repo)
Elements from [Mixamo](https://www.mixamo.com/) and [Quaternius](https://quaternius.com/).  
![example1](sample1.png)  

Sponza (Dabrovic, Meinl, McGuire, Hansen) with elements from [Mixamo](https://www.mixamo.com/).  
![example1](sample4.png)  

<!--
[Tarisland by Doctor A.](https://sketchfab.com/3d-models/tarisland-dragon-high-poly-ecf63885166c40e2bbbcdf11cd14e65f)  
![example2](sample2.png)  
-->

## Other credits
- GLDebugMessageCallback by [Plasmoxy](https://gist.github.com/Plasmoxy/aec637b85e306f671339dcfd509efc82) and [liam-middlebrook](https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f)
- [entt-meets-sol2](https://github.com/skaarj1989/entt-meets-sol2)
