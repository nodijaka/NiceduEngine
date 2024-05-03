
#include "config.h"
#include "glcommon.h"
// #include <GL/glew.h>
// #ifdef __APPLE__
// #include <OpenGL/gl.h>
// #else
// #include <windows.h>
// #include <GL/gl.h>
// #endif

// OpenGL debug message callback requires 4.3
#ifdef EENG_GLVERSION_43
#include "GLDebugMessageCallback.h"
#endif

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include "UILog.hpp"
#include "RenderableMesh.hpp"
#include "ForwardRenderer.hpp"

using linalg::m4f;
using linalg::m3f;
using linalg::v3f;

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;
float FRAMETIME_MIN_MS = 1000.0f / 60;
bool WIREFRAME = false;
float ANIM_SPEED = 1.0f;
bool SOUND_PLAY = false;

int main(int argc, char *argv[])
{
    //    EENG_ASSERT(false, "Debug break test {0}", 123);
    auto renderer = std::make_shared<eeng::ForwardRenderer>();

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

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    // OpenGL debug output callback
#ifdef EENG_GLVERSION_43
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugMessageCallback, nullptr);
#endif

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

    // Grass
    auto grassMesh = std::make_shared<eeng::RenderableMesh>();
    grassMesh->load("assets/grass/grass_trees.fbx", false);

    auto characterMesh = std::make_shared<eeng::RenderableMesh>();
#if 0
    // Character
    characterMesh->load("assets/Ultimate Platformer Pack/Character/Character.fbx", false);
#endif
#if 0
    // Enemy
    characterMesh->load("assets/Ultimate Platformer Pack/Enemies/Bee.fbx", false);
#endif
#if 0
    // Dragon
    // Requires MaxBones = 151;
    // https://sketchfab.com/3d-models/tarisland-dragon-high-poly-ecf63885166c40e2bbbcdf11cd14e65f
    // characterMesh->load("assets/tarisland-dragon-high-poly/M_B_44_Qishilong_skin_Skeleton.FBX");
#endif
#if 0
    // ExoRed 5.0.1 PACK FBX, 60fps, No keyframe reduction
    characterMesh->load("assets/ExoRed/exo_red.fbx");
    characterMesh->load("assets/ExoRed/idle (2).fbx", true);
    characterMesh->load("assets/ExoRed/walking.fbx", true);
    // Remove root motion
    characterMesh->remove_translation_keys("mixamorig:Hips");
#endif
#if 0
    // Amy 5.0.1 PACK FBX
    characterMesh->load("assets/Amy/Ch46_nonPBR.fbx");
    characterMesh->load("assets/Amy/idle.fbx", true);
    characterMesh->load("assets/Amy/walking.fbx", true);
    // Remove root motion
    characterMesh->remove_translation_keys("mixamorig:Hips");
#endif
#if 1
    // Eve 5.0.1 PACK FBX
    characterMesh->load("assets/Eve/Eve By J.Gonzales.fbx");
    characterMesh->load("assets/Eve/idle.fbx", true);
    characterMesh->load("assets/Eve/walking.fbx", true);
    // Remove root motion
    characterMesh->remove_translation_keys("mixamorig:Hips");
#endif

    LOG_DEFINES<eeng::UILog>();
    renderer->init("shaders/phong_vert.glsl","shaders/phong_frag.glsl");
    eeng::UILog::log("Entering main loop...");

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
        
        // Render GUI here

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

        eeng::UILog::draw();

        renderer->beginPass();
        // renderer->renderMesh()
        renderer->endPass();

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

        // linalg::m4f W = mat4f::TRS({0,0,0}, time_s *  0.75f, {0,1,0}, {0.1f,0.1f,0.1f}); // Leela
        //  linalg::m4f W = mat4f::TRS({0,0,0}, animtime*0.01f * 0, {1,0,0}, {0.25f,0.25f,0.25f}); // Manneq
        // linalg::m4f W = mat4f::TRS({0, -50, 0}, time_s * 0.75f, {0, 1, 0}, {0.15f, 0.15f, 0.15f}); // Character
        //  linalg::m4f W = mat4f::TRS({0,0,0}, animtime*0.01f, {0,1,0}, {0.1f,0.1f,0.1f}); // Kenney
        //  linalg::m4f W = mat4f::TRS({0,0,0}, animtime*0.01f, {0,1,0}, {0.4f,0.4f,0.4f}); // Mixamo Eve
        linalg::m4f W = m4f::TRS({0, -50, 0}, time_s * 0.75f, {0, 1, 0}, {0.6f, 0.6f, 0.6f}); // Mixamo
        // linalg::m4f W = mat4f::TRS({0, -50, 0}, time_s * 0.75f, {0, 1, 0}, {50.0f, 50.0f, 50.0f}); // DAE
        // linalg::m4f W = mat4f::TRS({0, -40, 0}, time_s * 0.75f, {0, 1, 0}, {0.05f, 0.05f, 0.05f}); // Dragon
        characterMesh->render(P * V, W, time_s * ANIM_SPEED, -1, lightPos, eye);
#if 1
        W = m4f::TRS({-30, 0, 0}, 0.0f, {0, 1, 0}, {1.0f, 1.0f, 1.0f}) * W; // Amy
        characterMesh->render(P * V, W, time_s * ANIM_SPEED, 1, lightPos, eye);
        W = m4f::TRS({60, 0, 0}, 0.0f, {0, 1, 0}, {1.0f, 1.0f, 1.0f}) * W; // Amy
        characterMesh->render(P * V, W, time_s * ANIM_SPEED, 2, lightPos, eye);
#endif

        // Animate & render grass
        W = m4f::TRS({-50.0f, -50.0f, -150.0f}, 0.0f, {0, 1, 0}, {100.0f, 100.0f, 100.0f});
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

    eeng::UILog::log("Exiting...");

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
