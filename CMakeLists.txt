
# https://github.com/SFML/cmake-sfml-project/blob/imgui-sfml/CMakeLists.txt

cmake_minimum_required(VERSION 3.5)

project(MyProgram
  VERSION 1.0
  LANGUAGES CXX
)

# Check for & fetch dependencies

include(FetchContent)

# Fetch SFML
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

# Fetch STB
FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)

# Fetch Dear ImGui
FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.89.9)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

# Fetch ImGui-SFML
set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(ImGui-SFML)

# Include directories for ImGui and ImGui-SFML
include_directories(
    ${ImGui_SOURCE_DIR}
    ${ImGui-SFML_SOURCE_DIR}
)

# Create targets

# Module1
add_executable(Module1 main.cpp)
set_target_properties(Module1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Module1"
)
target_link_libraries(Module1 PRIVATE sfml-graphics sfml-window sfml-system ImGui-SFML::ImGui-SFML)

# Module2
add_executable(Module2 main.cpp)
set_target_properties(Module2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Module2"
)
target_link_libraries(Module2 PRIVATE sfml-graphics sfml-window sfml-system ImGui-SFML::ImGui-SFML)
