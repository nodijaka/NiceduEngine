#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <windows.h>
#include <GL/gl.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>

// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include "RenderableMesh.hpp"

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;
float FRAMETIME_MIN_MS = 1000.0f / 60;
bool WIREFRAME = false;
float ANIM_SPEED = 1.0f;
bool SOUND_PLAY = false;

int main(int argc, char *argv[])
{
    // Hello standard output
    std::cout << "Hello SDL2 + Assimp + Dear ImGui" << std::endl;

    // Initialize SDL
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // OpenGL context attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Create a window
    SDL_Window *window = SDL_CreateWindow("SDL2 + Assimp + Dear ImGui",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create an OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Make the OpenGL context current
    if (SDL_GL_MakeCurrent(window, gl_context) != 0)
    {
        std::cerr << "Failed to make OpenGL context current: " << SDL_GetError() << std::endl;
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Check for OpenGL errors before initializing ImGui
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error before initializing ImGui: " << error << std::endl;
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    const char *glsl_version = "#version 410 core";
    if (!ImGui_ImplOpenGL3_Init(glsl_version))
    {
        std::cerr << "Failed to initialize ImGui with OpenGL backend" << std::endl;
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Check for OpenGL errors after initializing ImGui
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error after initializing ImGui: " << error << std::endl;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

#if 1
    // Load and play an audio clip
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8 *wavBuffer;
    SDL_AudioDeviceID deviceId = 0; // Declare deviceId outside of the if block
    // Load sound
    std::cout << "Playing sound..." << std::endl;
    if (SDL_LoadWAV("assets/sound/Juhani Junkala [Retro Game Music Pack] Title Screen.wav", &wavSpec, &wavBuffer, &wavLength) == NULL)
    {
        std::cerr << "Failed to load audio: " << SDL_GetError() << std::endl;
    }
    else
    {
        deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
        if (deviceId == 0)
        {
            std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        }
        else
        {
            // Enqueue the same sound ten times
            for (int i = 0; i < 10; i++)
                SDL_QueueAudio(deviceId, wavBuffer, wavLength);
            // SDL_PauseAudioDevice(deviceId, 0);
        }
    }
#endif

    /*
        // Load a model using Assimp
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile("/Users/ag1498/Dropbox/dev/assets/meshes/quaternius/FBX/Leela.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
        if (!scene) {
            std::cerr << "Failed to load model: " << importer.GetErrorString() << std::endl;
        } else
        {
            std::cout << "Assimp loaded file successfully..." << std::endl;
        }
    */

    auto grassMesh = std::make_shared<RenderableMesh>();
    auto characterMesh = std::make_shared<RenderableMesh>();

    grassMesh->load("assets/grass/grass_trees.fbx", false);

    // treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Sketchfab/maple-tree/maple-tree.fbx");
    // treemesh->load("assets/Leela.fbx", false);
    // treemesh->load("assets/Character.fbx", false);
#if 0
    treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Kenney Game Assets All-in-1 2.0.0/3D assets/Animated Characters Bundle/Models/characterLargeFemale.fbx", false);
    treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Kenney Game Assets All-in-1 2.0.0/3D assets/Animated Characters Bundle/Animations/idle.fbx", true);
    treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Kenney Game Assets All-in-1 2.0.0/3D assets/Animated Characters Bundle/Animations/run.fbx", true);
    treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Kenney Game Assets All-in-1 2.0.0/3D assets/Animated Characters Bundle/Animations/walk.fbx", true);
#endif
#if 1
    // Amy
    characterMesh->load("assets/Amy/Ch46_nonPBR.fbx");
    characterMesh->load("assets/Amy/Walking.fbx", true);
    characterMesh->load("assets/Amy/Talking.fbx", true);
    characterMesh->load("assets/Amy/Warrior Idle.fbx", true);
#endif
#if 0
            // EVE - Mixamo Pro Rifle Pack
            treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Mixamo/Eve_ProRiflePack/eve_j_gonzales.fbx");
            treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Mixamo/Eve_ProRiflePack/clips/idle.fbx", true);
            treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Mixamo/Eve_ProRiflePack/clips/walk forward.fbx", true);
            treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Mixamo/Eve_ProRiflePack/clips/walk right.fbx", true);
            treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Mixamo/Eve_ProRiflePack/clips/walk forward right.fbx", true);
            treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/Mixamo/Eve_ProRiflePack/clips_extra/IdleReload.fbx", true);
#endif
    // treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/UE4/SK_Mannequin_tex.FBX");
    // treemesh->load("/Users/ag1498/Dropbox/dev/assets/meshes/UE4/clips_animpack/Aim_Space_Ironsights_PreviewMesh.fbx", true);

    // Timestamp of last frame
    // Uint32 lastFrame = SDL_GetTicks();
    // Desired frame time
    // Uint32 frameDelay = 1000 / FRAMETIME_MIN_MS;

    // Main loop
    float time_s, time_ms;
    bool quit = false;
    SDL_Event event;
    while (!quit)
    {
        time_ms = SDL_GetTicks();
        time_s = time_ms * 0.001f;

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event); // Send events to ImGui
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
        // Render your GUI here
        ImGui::Begin("Config");

        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        // Combo (drop-down) for fps settings
        static const char *items[] = {"10", "30", "60", "120", "Uncapped"};
        static int currentItem = 2;
        if (ImGui::BeginCombo("Target framerate##targetfps", items[currentItem]))
        {
            for (int i = 0; i < IM_ARRAYSIZE(items); i++)
            {
                const bool isSelected = (currentItem == i);
                if (ImGui::Selectable(items[i], isSelected))
                    currentItem = i;

                if (isSelected)
                    ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo
            }
            ImGui::EndCombo();
        }
        if (currentItem == 0)
            FRAMETIME_MIN_MS = 1000.0f / 10;
        else if (currentItem == 1)
            FRAMETIME_MIN_MS = 1000.0f / 30;
        else if (currentItem == 2)
            FRAMETIME_MIN_MS = 1000.0f / 60;
        else if (currentItem == 3)
            FRAMETIME_MIN_MS = 1000.0f / 120;
        else if (currentItem == 4)
            FRAMETIME_MIN_MS = 0.0f;

        ImGui::Checkbox("Wireframe rendering", &WIREFRAME);

        ImGui::SliderFloat("Animation speed", &ANIM_SPEED, 0.1f, 5.0f);

        if (SOUND_PLAY)
        {
            if (ImGui::Button("Pause sound"))
            {
                SDL_PauseAudioDevice(deviceId, 1);
                SOUND_PLAY = false;
            }
        }
        else
        {
            if (ImGui::Button("Play sound"))
            {
                SDL_PauseAudioDevice(deviceId, 0);
                SOUND_PLAY = true;
            }
        }

        ImGui::End();

        // Face culling - takes place before rasterization
        glEnable(GL_CULL_FACE); // Perform face culling
        glFrontFace(GL_CCW);    // Define winding for a front-facing face
        glCullFace(GL_BACK);    // Cull back-facing faces
        // Rasterization stuff
        glEnable(GL_DEPTH_TEST); // Perform depth test when rasterizing
        glDepthFunc(GL_LESS);    // Depth test pass if z < existing z (closer than existing z)
        glDepthMask(GL_TRUE);    // If depth test passes, write z to z-buffer
        glDepthRange(0, 1);      // Z-buffer range is [0,1], where 0 is at z-near and 1 is at z-far

        // Define viewport transform = Clip -> Screen space (applied before rasterization)
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

        // Bind the default framebuffer (only needed when using multiple render targets)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Clear depth and color attachments of frame buffer
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (WIREFRAME)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);
        }

        linalg::v3f lightPos = m3f::rotation(time_s * 0.0f, 1.0f, 0.0f, 0.0f) * v3f{1000.0f, 1000.0f, 1000.0f};
        linalg::v3f eye{0.0f, 0.0f, 100.0f};

        // animtime += 0.016f;
        //
        //  glViewport
        //  http://www.glfw.org/docs/latest/quick.html#quick_render
        //  int width, height; // ~2x the window size on Mac Retina
        //  glfwGetFramebufferSize(window, &width, &height);
        //  glViewport(0, 0, w, h);
        //
        float fov = 60.0f * fTO_RAD;
        float aspect = float(WINDOW_WIDTH) / WINDOW_HEIGHT;
        linalg::m4f P = linalg::m4f::GL_PerspectiveProjectionRHS(fov, aspect, 1.0f, 500.0f);
        linalg::m4f V = linalg::m4f::TRS(eye, 0.0f, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}).inverse();

        // linalg::m4f W = mat4f::TRS({0,0,0}, time_s *  0.5f, {0,1,0}, {0.1f,0.1f,0.1f}); // Leela
        //  linalg::m4f W = mat4f::TRS({0,0,0}, animtime*0.01f * 0, {1,0,0}, {0.25f,0.25f,0.25f}); // Manneq
        // linalg::m4f W = mat4f::TRS({0,0,0}, time_s *  0.5f, {0,1,0}, {0.25f,0.25f,0.25f}); // Character
        //  linalg::m4f W = mat4f::TRS({0,0,0}, animtime*0.01f, {0,1,0}, {0.1f,0.1f,0.1f}); // Kenney
        //  linalg::m4f W = mat4f::TRS({0,0,0}, animtime*0.01f, {0,1,0}, {0.4f,0.4f,0.4f}); // Mixamo Eve
        linalg::m4f W = mat4f::TRS({0, -50, 0}, time_s * 0.5f, {0, 1, 0}, {0.6f, 0.6f, 0.6f}); // Amy
        characterMesh->render(P * V, W, time_s * ANIM_SPEED, 0, lightPos, eye);

        W = mat4f::TRS({20, -50, 0}, time_s * 0.5f, {0, 1, 0}, {0.6f, 0.6f, 0.6f}); // Amy
        characterMesh->render(P * V, W, time_s * ANIM_SPEED, 1, lightPos, eye);
        W = mat4f::TRS({40, -50, 0}, time_s * 0.5f, {0, 1, 0}, {0.6f, 0.6f, 0.6f}); // Amy
        characterMesh->render(P * V, W, time_s * ANIM_SPEED, 2, lightPos, eye);

        // Animate & render grass
        W = mat4f::TRS({-50.0f, -50.0f, -150.0f}, 0.0f, {0, 1, 0}, {100.0f, 100.0f, 100.0f});
        grassMesh->render(P * V, W, 0.0f, -1, lightPos, eye);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);

        // Add a delay if frame time was faster than the target frame time
        const Uint32 elapsed_ms = SDL_GetTicks() - time_ms;
        if (elapsed_ms < FRAMETIME_MIN_MS)
            SDL_Delay(FRAMETIME_MIN_MS - elapsed_ms);

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
