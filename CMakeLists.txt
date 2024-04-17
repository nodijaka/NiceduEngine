
# https://github.com/SFML/cmake-sfml-project/blob/imgui-sfml/CMakeLists.txt

cmake_minimum_required(VERSION 3.5)

project(DA376B_Labs
  VERSION 1.0
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check this
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/vec)

find_package(OpenGL REQUIRED)
find_package (Git REQUIRED)
include(FetchContent)

# Fetch SDL
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.30.2 # Replace this with the desired version/tag
)
FetchContent_GetProperties(SDL)
if(NOT SDL_POPULATED)
    FetchContent_Populate(SDL)
    add_subdirectory(${sdl_SOURCE_DIR} ${sdl_BINARY_DIR})
endif()
include_directories(${sdl_SOURCE_DIR}/include)

# Fetch SFML
#FetchContent_Declare(
#  SFML
#  GIT_REPOSITORY https://github.com/SFML/SFML.git
#  GIT_TAG 2.6.x
#)
#FetchContent_MakeAvailable(SFML)

# Fetch Assimp
# Note: v5.0.1 (200112) seems to be the latest version of assimp that works with Mixamo models (https://github.com/assimp/assimp/issues/4620)
#FetchContent_Declare(
#    assimp
#    GIT_REPOSITORY https://github.com/assimp/assimp.git
#    GIT_TAG        v5.1.0 # Replace this with the desired version/tag
#)
#FetchContent_GetProperties(assimp)
#if(NOT assimp_POPULATED)
#    FetchContent_Populate(assimp)
#    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable Assimp tests" FORCE)
#    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Disable Assimp tools" FORCE)
#    add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
#endif()
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Assimp/CMakeLists.txt")
    add_subdirectory("assimp")
else()
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.0.1 # v5.4.0 # (240408) 5.3.0 (230923) fixes issues with vsprintf and other deprecated stuff
        GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(assimp)
endif()
#if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_COMPILER_ID STREQUAL "GNU")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-declarations -Wno-unused-but-set-variable")
#endif()

# Fetch STB
FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)
include_directories(${stb_SOURCE_DIR})

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
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.89.9)
FetchContent_MakeAvailable(imgui)
#FetchContent_GetProperties(imgui SOURCE_DIR IMGUI_DIR)

# Fetch ImGui-SFML
#set(IMGUI_SFML_FIND_SFML OFF)
#FetchContent_Declare(ImGui-SFML
#    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
#    GIT_TAG 2.6.x)
#FetchContent_MakeAvailable(ImGui-SFML)

# Include directories for ImGui and ImGui-SFML
include_directories(${imgui_SOURCE_DIR})

# Create targets

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vec/mat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vec/vec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vec/quat.cpp
    )

set_target_properties(Module1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Module1"
)
target_link_libraries(Module1 SDL2 assimp ${OPENGL_LIBRARIES}) #SDL2main SDL assimp)
target_include_directories(Module1 PRIVATE ${imgui_SOURCE_DIR})
target_include_directories(Module1 PRIVATE ${imgui_SOURCE_DIR}/backends)
#target_compile_features(Module1 PRIVATE cxx_std_17)
# Copy OpenAL DLL to the target directory on Windows
#if(WIN32)
#    add_custom_command(
#        TARGET Module1
#        COMMENT "Copy OpenAL DLL"
#        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Module1>
#        VERBATIM)
#endif()

# Module2
#add_executable(Module2
#    main.cpp 
#    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
#    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
#set_target_properties(Module2 PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Module2"
#)
#target_link_libraries(Module2 SDL2 assimp ${OPENGL_LIBRARIES}) #SDL2main SDL2 assimp)
#target_include_directories(Module2 PRIVATE ${imgui_SOURCE_DIR})
#target_include_directories(Module2 PRIVATE ${imgui_SOURCE_DIR}/backends)

# For 'make install'
#install(TARGETS Module1 Module2)