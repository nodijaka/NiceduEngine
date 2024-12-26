#ifndef Scene_hpp
#define Scene_hpp
#pragma once

// #include <entt/entt.hpp> // -> Scene source
#include <entt/fwd.hpp>
#include "SceneBase.h"
#include "RenderableMesh.hpp"
#include "ForwardRenderer.hpp"
#include "ShapeRenderer.hpp"

/// @brief TODO
class Scene : public eeng::SceneBase
{
public:

    /// @brief TODO
    /// @return 
    bool init() override;

    /// @brief TODO
    /// @param time_s 
    /// @param deltaTime_s 
    void update(
        float time_s,
        float deltaTime_s,
        InputManagerPtr input) override;

    /// @brief 
    void renderUI() override;

    /// @brief TODO
    /// @param time_s 
    /// @param screenWidth 
    /// @param screenHeight 
    /// @param renderer 
    void render(
        float time_s,
        int screenWidth,
        int screenHeight) override;

    /// @brief TODO
    void destroy() override;

private:

    // Renderer for rendering imported animated or non-animated models
    eeng::ForwardRendererPtr forwardRenderer;

    // Immediate-mode renderer for basic 2D or 3D primitives
    ShapeRendererPtr shapeRenderer;

    // Entity registry - to use in labs
    std::shared_ptr<entt::registry> entity_registry;

    // Camera properties
    glm::vec3 eyePos, eyeAt, eyeUp{ 0.0f, 1.0f, 0.0f };
    const float nearPlane = 1.0f, farPlane = 500.0f;
    float sensitivity = 0.005f;
    float yaw = 0.0f;   // Horizontal angle (radians)
    float pitch = 0.0f; // Vertical angle (radians)
    float radius = 15.0f;
    glm::vec2 mouse_xy_prev{ -1.0f, -1.0f };

    // Light properties
    glm::vec3 lightPos, lightColor{ 1.0f, 1.0f, 0.8f };

    // Scene objects
    std::shared_ptr<eeng::RenderableMesh> grassMesh, horseMesh, characterMesh;

    // Velocity placeholder
    float player_velocity {0.1f};
    glm::vec3 player_pos = glm_aux::vec3_000;

    // Scene object transformations
    glm::mat4 characterWorldMatrix1, characterWorldMatrix2, characterWorldMatrix3;
    glm::mat4 grassWorldMatrix, horseWorldMatrix;

    int characterAnimIndex = -1;
    float characterAnimSpeed = 1.0f;
    int drawcallCount = 0;

    void updateCameraRotation(float deltaX, float deltaY);
};

#endif