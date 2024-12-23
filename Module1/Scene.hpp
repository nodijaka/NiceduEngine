#ifndef Scene_hpp
#define Scene_hpp
#pragma once

#include <entt/entt.hpp> // -> Scene source
#include "SceneBase.h"
#include "RenderableMesh.hpp"

#include "ShapeRenderer.hpp" // -> main?

/// @brief TODO
class Scene : public eeng::SceneBase
{
protected:
    ShapeRendering::ShapeRenderer shapeRenderer {};

    // Entity registry - to use in labs
    entt::registry registry;

    // Camera properties
    glm::vec3 eyePos, atPos, upVector {0.0f, 1.0f, 0.0f};
    const float nearPlane = 1.0f, farPlane = 500.0f;

    // Light properties
    glm::vec3 lightPos, lightColor{ 1.0f, 1.0f, 0.8f };

    // Scene objects
    std::shared_ptr<eeng::RenderableMesh> grassMesh, horseMesh, characterMesh;
    
    // Scene object transformations
    glm::mat4 characterWorldMatrix1, characterWorldMatrix2, characterWorldMatrix3;
    glm::mat4 grassWorldMatrix, horseWorldMatrix;

    int characterAnimIndex = -1;
    float characterAnimSpeed = 1.0f;
    int drawcallCount = 0;

public:

    /// @brief TODO
    /// @return 
    bool init() override;

    /// @brief TODO
    /// @param time_s 
    /// @param deltaTime_s 
    void update(float time_s, float deltaTime_s) override;

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
        int screenHeight,
        eeng::ForwardRendererPtr renderer) override;

    /// @brief TODO
    void destroy() override;
};

#endif