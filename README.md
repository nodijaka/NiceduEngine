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
> **Note:** A clean build will take some time to complete since all code is compiled from source.

### Windows
> **Note:** (Visual Studio) If step 4 does not build correctly, try opening the solution (which was generated in step 3) and build from within Visual Studio instead.
```sh
# 1. Open PowerShell and clone the repository
git clone https://github.com/cjgribel/eduEngine.git

# 2. Navigate to the project directory
cd eduEngine

# (Optional: Verify that CMake is found)
cmake --version

# (Debug Build)
# 3. Generate project
cmake -B Debug -DCMAKE_BUILD_TYPE=Debug
# 4. Build project
cmake --build Debug

# (Release Build)
# 3. Generate project
cmake -B Release -DCMAKE_BUILD_TYPE=Release
# 4. Build project
cmake --build Release
```

### macOS
```sh
# Open a terminal and clone the repository
git clone https://github.com/cjgribel/eduEngine.git

# Navigate to the project directory
cd eduEngine

# (Optional: Verify that CMake is found)
cmake --version

# Debug Build
cmake -B Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build Debug

# Release Build
cmake -B Release -DCMAKE_BUILD_TYPE=Release
cmake --build Release
```

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
Test scene with elements from [Mixamo](https://www.mixamo.com/) and [Quaternius](https://quaternius.com/).  
![example1](example1.png)  

<!--
[Tarisland by Doctor A.](https://sketchfab.com/3d-models/tarisland-dragon-high-poly-ecf63885166c40e2bbbcdf11cd14e65f)  
![example2](example2.png)  
-->

## Other credits
- GLDebugMessageCallback by [Plasmoxy](https://gist.github.com/Plasmoxy/aec637b85e306f671339dcfd509efc82) and [liam-middlebrook](https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f)
- [entt-meets-sol2](https://github.com/skaarj1989/entt-meets-sol2)

_Updated 250204_  
