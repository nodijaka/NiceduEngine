// Licensed under the MIT License. See LICENSE file for details.

#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <iostream>
#include <memory>
#include "config.h"
#include "GameBase.h"

struct SDL_Window;              // Forward declaration
typedef void* SDL_GLContext;    // Forward declaration

namespace eeng
{
    class InputManager; // Forward declaration

/**
 * @brief Main engine class handling SDL, OpenGL, ImGui initialization and the main loop.
 */
class Engine
{
public:
    /** Constructor */
    Engine();

    /** Destructor, calls shutdown() */
    ~Engine();

    /**
     * @brief Initialize the engine.
     * @param title Window title
     * @param width Window width
     * @param height Window height
     * @return True if successful, false otherwise
     */
    bool init(const char* title, int width, int height);

    /**
     * @brief Start the main loop.
     * @param game Unique pointer to the initial game game
     */
    void run(std::unique_ptr<GameBase> game);

    /** @brief Clean up and close the engine. */
    void shutdown();

    /**
     * @brief Get SDL window pointer.
     * @return SDL_Window pointer
     */
    SDL_Window* window() const { return window_; }

private:
    SDL_Window* window_ = nullptr;        ///< SDL Window pointer
    SDL_GLContext gl_context_ = nullptr;  ///< OpenGL context
    std::shared_ptr<InputManager> input;  ///< Input manager for mouse/keyboard/controller input

    int window_height;    ///< Window height in pixels
    int window_width;     ///< Window width in pixels
    bool vsync = false;   ///< V-sync enabled state
    bool wireframe_mode = false; ///< Wireframe rendering state
    float min_frametime_ms = 16.67; ///< Minimum frame duration in milliseconds (default 60 FPS)

    /** Initialize SDL library and window. */
    bool init_sdl(const char* title, int width, int height);

    /** Initialize OpenGL context. */
    bool init_opengl();

    /** Initialize ImGui for GUI rendering. */
    bool init_imgui();

    /** Handle SDL events. */
    void process_events(bool& running);

    /** Prepare frame rendering. */
    void begin_frame();

    /** Finish frame rendering. */
    void end_frame();

    /** Render debug and information UI using ImGui. */
    void render_info_UI();
};

} // namespace eeng

#endif // ENGINE_HPP