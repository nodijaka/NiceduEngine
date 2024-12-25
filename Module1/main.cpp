
#include <iostream>
#include "config.h"
#include "glcommon.h"
#ifdef EENG_GLVERSION_43
// OpenGL debug message callback requires 4.3
#include "GLDebugMessageCallback.h"
#endif

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include "InputManager.hpp"
#include "Log.hpp"
#include "Scene.hpp"

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;
float FRAMETIME_MIN_MS = 1000.0f / 60;
bool WIREFRAME = false;
bool SOUND_PLAY = false;
SDL_GameController* controller1;

namespace
{
    // Helpers

    SDL_GameController* findController()
    {
        for (int i = 0; i < SDL_NumJoysticks(); i++)
        {
            if (SDL_IsGameController(i))
            {
                return SDL_GameControllerOpen(i);
            }
        }

        return nullptr;
    }
}

int main(int argc, char* argv[])
{

    // Hello standard output
    std::cout << "Hello SDL2 + Assimp + Dear ImGui" << std::endl;

    // Initialize SDL
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Controllers
    controller1 = findController();

    // OpenGL context attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, EENG_GLVERSION_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, EENG_GLVERSION_MINOR);
#ifdef EENG_MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, EENG_MSAA_SAMPLES);
#endif

    // Create a window
    SDL_Window* window = SDL_CreateWindow("SDL2 + Assimp + Dear ImGui",
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
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    const char* glsl_version = "#version 410 core";
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
    Uint8* wavBuffer;
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

    // Log some state
    LOG_DEFINES<eeng::Log>();
    {
        int glMinor, glMajor;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &glMinor);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &glMajor);
        eeng::Log::log("GL version %i.%i (requested), %i.%i (actual)", EENG_GLVERSION_MAJOR, EENG_GLVERSION_MINOR, glMajor, glMinor);
    }
#ifdef EENG_MSAA
    {
        int actualMSAA;
        SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &actualMSAA);
        eeng::Log::log("MSAA %i (requested), %i (actual)", EENG_MSAA_SAMPLES, actualMSAA);
    }
#endif
#ifdef EENG_ANISO
    {
        GLfloat maxAniso;
#if defined(EENG_GLVERSION_43)
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
#elif defined(EENG_GLVERSION_41)
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
#endif
        eeng::Log::log("Anisotropic samples %i (requested), %i (max))", EENG_ANISO_SAMPLES, (int)maxAniso);
    }
#endif

    auto input = std::make_shared<eeng::InputManager>();

    auto scene = std::make_shared<Scene>();
    scene->init();

    // Main loop
    float time_s = 0.0f, time_ms, deltaTime_s = 0.016f;
    bool quit = false;
    SDL_Event event;
    eeng::Log::log("Entering main loop...");

    while (!quit)
    {
        const auto now_ms = SDL_GetTicks();
        const auto now_s = now_ms * 0.001f;
        deltaTime_s = now_s - time_s;
        time_ms = now_ms;
        time_s = now_s;

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event); // Send events to ImGui

            input->HandleEvent(&event);

            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
                // case SDL_CONTROLLERDEVICEADDED:
                //     if (!controller1)
                //     {
                //         controller1 = SDL_GameControllerOpen(event.cdevice.which);
                //     }
                //     break;
                // case SDL_CONTROLLERDEVICEREMOVED:
                //     if (controller1 && event.cdevice.which == SDL_JoystickInstanceID(
                //         SDL_GameControllerGetJoystick(controller1)))
                //     {
                //         SDL_GameControllerClose(controller1);
                //         controller1 = findController();
                //     }
                //     break;
                // case SDL_CONTROLLERBUTTONDOWN:
                //     break;
            }
        }

        if (SDL_GameControllerGetButton(controller1, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X))
        {
            // printf("X was pressed!\n");
        }

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        // Render GUI here

        ImGui::Begin("Info");

        if (ImGui::CollapsingHeader("Backend", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            // ImGui::Text("Drawcall count %i", DRAWCALL_COUNT);

            // Combo (drop-down) for fps settings
            static const char* items[] = { "10", "30", "60", "120", "Uncapped" };
            static int currentItem = 2;
            if (ImGui::BeginCombo("Target framerate##targetfps", items[currentItem]))
            {
                for (int i = 0; i < IM_ARRAYSIZE(items); i++)
                {
                    const bool isSelected = (currentItem == i);
                    if (ImGui::Selectable(items[i], isSelected))
                        currentItem = i;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
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

            ImGui::Text("Controllers connected: %i", input->GetConnectedControllerCount());

            for (auto& [id, state] : input->get_controllers())
            {
                //const auto& controller = input->GetControllerState(i);

                ImGui::PushID(id);
                ImGui::BeginChild("Controller", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 6), true);

                ImGui::Text("Controller %i: '%s'", id, state.name.c_str());
                ImGui::Text("Left stick:  X: %.2f  Y: %.2f", state.axisLeftX, state.axisLeftY);
                ImGui::Text("Right stick: X: %.2f  Y: %.2f", state.axisRightX, state.axisRightY);
                ImGui::Text("Triggers:    L: %.2f  R: %.2f", state.triggerLeft, state.triggerRight);
                std::string buttons;
                for (const auto& [buttonId, isPressed] : state.buttonStates)
                    buttons += "#" + std::to_string(buttonId) + "(" + (isPressed ? "1) " : "0) ");
                ImGui::Text("Buttons: %s", buttons.c_str());

                ImGui::EndChild();
                ImGui::PopID();
            }

            // if (controller1 != nullptr)
            // {
            //     ImGui::BeginChild("Controller State Frame", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 4), true);
            //     ImGui::Text("Buttons: A:%d B:%d X:%d Y:%d",
            //         SDL_GameControllerGetButton(controller1, SDL_CONTROLLER_BUTTON_A),
            //         SDL_GameControllerGetButton(controller1, SDL_CONTROLLER_BUTTON_B),
            //         SDL_GameControllerGetButton(controller1, SDL_CONTROLLER_BUTTON_X),
            //         SDL_GameControllerGetButton(controller1, SDL_CONTROLLER_BUTTON_Y));

            //     ImGui::Text("Left Stick: X:%.2f Y:%.2f",
            //         SDL_GameControllerGetAxis(controller1, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f,
            //         SDL_GameControllerGetAxis(controller1, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f);

            //     ImGui::Text("Right Stick: X:%.2f Y:%.2f",
            //         SDL_GameControllerGetAxis(controller1, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f,
            //         SDL_GameControllerGetAxis(controller1, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f);
            //     ImGui::EndChild();
            // }
            // else
            // {
            //     ImGui::SameLine();
            //     ImGui::Text("(No controller connected)");
            // }
        }

        if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
        {
            scene->renderUI();
        }

        ImGui::End(); // end info window

        eeng::Log::draw();

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
        // glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
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

        input->Update();

        scene->update(time_s, deltaTime_s, input);
        scene->render(time_s, WINDOW_WIDTH, WINDOW_HEIGHT);

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

    eeng::Log::log("Exiting...");

    // Cleanup
    scene->destroy();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
