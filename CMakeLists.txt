
# https://github.com/SFML/cmake-sfml-project/blob/imgui-sfml/CMakeLists.txt

cmake_minimum_required(VERSION 3.5)

project(DA376B_Labs
  VERSION 1.0
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

#Fetch GLM
#FetchContent_Declare(
#  glm
#  GIT_REPOSITORY https://github.com/g-truc/glm.git
#  GIT_TAG       0.9.9.8  # Specify the version or tag you want to fetch
#)
#FetchContent_MakeAvailable(glm)
#if(glm_POPULATED)
#  add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
#endif()

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
target_link_libraries(Module1 PRIVATE sfml-graphics sfml-window sfml-system sfml-audio ImGui-SFML::ImGui-SFML)
#target_compile_features(Module1 PRIVATE cxx_std_17)
# Copy OpenAL DLL to the target directory on Windows
if(WIN32)
    add_custom_command(
        TARGET Module1
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Module1>
        VERBATIM)
endif()

# Module2
add_executable(Module2 main.cpp)
set_target_properties(Module2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Module2"
)
target_link_libraries(Module2 PRIVATE sfml-graphics sfml-window sfml-system sfml-audio ImGui-SFML::ImGui-SFML)
#target_compile_features(Module2 PRIVATE cxx_std_17)
# Copy OpenAL DLL to the target directory on Windows
if(WIN32)
    add_custom_command(
        TARGET Module2
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Module2>
        VERBATIM)
endif()

# For 'make install'
#install(TARGETS Module1 Module2)