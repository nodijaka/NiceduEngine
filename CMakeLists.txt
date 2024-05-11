cmake_minimum_required(VERSION 3.11)

#
# Project
#
project(DA376B_Labs
  VERSION 1.0
  LANGUAGES CXX
)

#
# CPP
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

#
# Compiler
#
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    add_compile_definitions(EENG_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#    add_compile_definitions(EENG_RELEASE)
endif()
if(APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION=1)
else()
    add_compile_definitions(GL_SILENCE_DEPRECATION=0)
endif()

# ???
# 'target_include_directories' if target specific
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/vec)

find_package(OpenGL REQUIRED)
find_package (Git REQUIRED)
include(FetchContent)

#
# Fetch SDL
#
FetchContent_Declare(
    sdl2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.30.2 # April 2nd 2024
)
FetchContent_GetProperties(sdl2)
if(NOT sdl2_POPULATED)
    FetchContent_Populate(sdl2)
    add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR})
endif()
include_directories(${sdl2_SOURCE_DIR}/include)

#
# Fetch glew
# https://github.com/Perlmint/glew-cmake/blob/master/glew-cmake/fetch-content.cmake
#
include(FetchContent)
FetchContent_Declare(
  glew
  GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
  GIT_TAG        origin/master
)
FetchContent_GetProperties(glew)
if(NOT glew_POPULATED)
  FetchContent_Populate(glew)
  add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

#
# Fetch Assimp
#
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Build zlib" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Build Assimp tools" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build Assimp tests" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Install Assimp" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "Inject debug postfix" FORCE)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Assimp/CMakeLists.txt")
    add_subdirectory("assimp")
else()
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.0.1 # 5.3.0 - v5.4.0 breaks some Mixamo animations
        GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(assimp)
endif()
include_directories(${assimp_SOURCE_DIR})
# Set C++14 standard for Assimp 5.0.1
#set_target_properties(assimp PROPERTIES
#    CXX_STANDARD 14
#    CXX_STANDARD_REQUIRED ON
#    CXX_EXTENSIONS OFF
#)

#
# Fetch STB
#
FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)
include_directories(${stb_SOURCE_DIR})

#
# Fetch GLM 
# https://github.com/g-truc/glm
#
# set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
# set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

#
# Fetch enTT (header-only)
#
FetchContent_Declare(
  entt
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        v3.13.2
)
FetchContent_MakeAvailable(entt)
include_directories(${entt_SOURCE_DIR}/src) 

#
# Fetch Dear ImGui
#
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.89.9)
FetchContent_MakeAvailable(imgui)
include_directories(${imgui_SOURCE_DIR})
include_directories(${imgui_SOURCE_DIR}/backends)

#
# Targets
#

# Module1
add_executable(Module1 
    main.cpp 
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RenderableMesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ForwardRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLDebugMessageCallback.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vec/mat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vec/vec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vec/quat.cpp
    )

set_target_properties(Module1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Module1"
)
target_link_libraries(Module1 PRIVATE SDL2 assimp libglew_static glm::glm ${OPENGL_LIBRARIES})
#target_include_directories(Module1 PRIVATE ${imgui_SOURCE_DIR})
#target_include_directories(Module1 PRIVATE ${imgui_SOURCE_DIR}/backends)
# Copy SDL2 DLL to the build directory (for Windows)
add_custom_command(TARGET Module1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2>
        $<TARGET_FILE_DIR:Module1>
)
# Copy assimp DLL to the build directory (for Windows)
add_custom_command(TARGET Module1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_FILE_DIR:assimp>"
        $<TARGET_FILE_DIR:Module1>
)

# Set working directory for Visual Studio
set_property(TARGET Module1 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Module2 ...




# Set startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Module1)

# For 'make install'
#install(TARGETS Module1 Module2)