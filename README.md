# eduEngine
Course framework for MAU::DA376B

## Requirements
- A compiler that supports C++17
- Source control: [git](https://git-scm.com/)
- Build system: [CMake](https://cmake.org/)
- Editor or IDE of choice, e.g. Visual Studio, Visual Studio Code, Xcode etc.

## Dependencies (fetched automatically)
- Backend for window, sound, input etc: [SDL2](https://github.com/libsdl-org/SDL)
- OpenGL extension loader: [glew-cmake](https://github.com/Perlmint/glew-cmake)
- Model loader: [assimp](https://github.com/assimp/assimp) v5.0.1 (200112, newer versions have [issues](https://github.com/assimp/assimp/issues/4620) with Mixamo models).
- User interface: [Dear ImGui](https://github.com/ocornut/imgui)
- Vector & matrix math: [glm](https://github.com/g-truc/glm)
- Image loader & writer: [stb](https://github.com/nothings/stb)
- Entity-Component-System: [EnTT](https://github.com/skypjack/entt)

## Under consideration

- Animation: [ozz-animation](https://guillaumeblanc.github.io/ozz-animation/)
- JSON reader & writer: [nlohmann-json](https://github.com/nlohmann/json)
- File-dialog for Dear ImGui: [ImGuiFileDialog](https://github.com/aiekick/ImGuiFileDialog)
- C++ binding for LUA: [sol2](https://github.com/ThePhD/sol2)

## Building Instructions

### Windows

```sh
# Clone the repository
git clone https://github.com/cjgribel/eduEngine.git

# Navigate to the project directory
cd eduEngine

# Debug Build
cmake -B Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build Debug

# Release Build
cmake -B Release -DCMAKE_BUILD_TYPE=Release
cmake --build Release
```

After successful build, you can find the executables in the respective directories (`Debug` or `Release`).

### macOS

```sh
# Clone the repository
git clone https://github.com/cjgribel/eduEngine.git

# Navigate to the project directory
cd eduEngine

# Debug Build
cmake -B Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build Debug

# Release Build
cmake -B Release -DCMAKE_BUILD_TYPE=Release
cmake --build Release
```

After successful build, you can find the executables in the respective directories (`Debug` or `Release`).

## Examples

![example2](example2.png)  
[Platformer Game Kit](https://quaternius.com/packs/ultimateplatformer.html)  

![example1](example1.png)  
[Tarisland by Doctor A.](https://sketchfab.com/3d-models/tarisland-dragon-high-poly-ecf63885166c40e2bbbcdf11cd14e65f)  

## Credits
- GLDebugMessageCallback by [Plasmoxy](https://gist.github.com/Plasmoxy/aec637b85e306f671339dcfd509efc82) and [liam-middlebrook](https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f)

Updated 240511  
