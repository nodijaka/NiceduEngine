#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

int main(int argc, char* argv[]) {

    std::cout << "Hello SDL + Assimp + ImGui" << std::endl;

    // Initialize SDL
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

   // Setup OpenGL context attributes
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Create a window
    SDL_Window* window = SDL_CreateWindow("SDL + Assimp + Dear ImGui", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create an OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Make the OpenGL context current
    if (SDL_GL_MakeCurrent(window, gl_context) != 0) {
        std::cerr << "Failed to make OpenGL context current: " << SDL_GetError() << std::endl;
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Check for OpenGL errors before initializing ImGui
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error before initializing ImGui: " << error << std::endl;
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

// Setup ImGui context
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO();
(void)io;

    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

// Setup Platform/Renderer bindings
ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
const char* glsl_version = "#version 410 core";
if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
    std::cerr << "Failed to initialize ImGui with OpenGL backend" << std::endl;
    //ImGui_ImplSDL2_Shutdown();
    //ImGui::DestroyContext();
    //SDL_GL_DeleteContext(gl_context);
    //SDL_DestroyWindow(window);
    //SDL_Quit();
    //return 1;
}

// Check for OpenGL errors after initializing ImGui
error = glGetError();
if (error != GL_NO_ERROR) {
    std::cerr << "OpenGL error after initializing ImGui: " << error << std::endl;
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplSDL2_Shutdown();
    //ImGui::DestroyContext();
    //SDL_GL_DeleteContext(gl_context);
    //SDL_DestroyWindow(window);
    //SDL_Quit();
    //return 1;
}

    // Load and play an audio clip
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8* wavBuffer;
    SDL_AudioDeviceID deviceId = 0; // Declare deviceId outside of the if block
    // Load sound
    std::cout << "Playing sound..." << std::endl;
    if (SDL_LoadWAV("/Users/ag1498/GitHub/eduEngine/assets/sound/Juhani Junkala [Retro Game Music Pack] Title Screen.wav", &wavSpec, &wavBuffer, &wavLength) == NULL) {
        std::cerr << "Failed to load audio: " << SDL_GetError() << std::endl;
    } else {
        deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
        if (deviceId == 0) {
            std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        } else {
            SDL_QueueAudio(deviceId, wavBuffer, wavLength);
            SDL_PauseAudioDevice(deviceId, 0);
        }
    }

    // Load a model using Assimp
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("/Users/ag1498/Dropbox/dev/assets/meshes/quaternius/FBX/Leela.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (!scene) {
        std::cerr << "Failed to load model: " << importer.GetErrorString() << std::endl;
    } else
    {
        std::cout << "Assimp loaded file successfully..." << std::endl;
    }

    // Main loop
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event); // Send events to ImGui
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
        // Render your GUI here
        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
        ImGui::End();

        // Rendering
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);

        // Example: Play the sound again after 5 seconds
//        SDL_Delay(5000);
//        if (!quit) {
//            SDL_QueueAudio(deviceId, wavBuffer, wavLength);
//            SDL_PauseAudioDevice(deviceId, 0);
//        }
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
