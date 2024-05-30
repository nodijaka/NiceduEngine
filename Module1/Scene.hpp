#ifndef Scene_hpp
#define Scene_hpp
#pragma once

#include <entt/entt.hpp> // -> Scene source
#include "SceneBase.h"
#include "RenderableMesh.hpp"

class Scene : public eeng::SceneBase
{
protected:
    entt::registry registry;

    std::shared_ptr<eeng::RenderableMesh> grassMesh, horseMesh, characterMesh;

    glm::mat4 characterWorldMatrix1, characterWorldMatrix2, characterWorldMatrix3;
    glm::mat4 grassWorldMatrix, horseWorldMatrix;

    glm::vec3 lightPos, eyePos;
    glm::vec3 lightColor{ 1.0f, 1.0f, 0.8f };

    const float nearPlane = 1.0f, farPlane = 500.0f;

    int characterAnimIndex = -1;
    float characterAnimSpeed = 1.0f;
    int drawcallCount = 0;

public:
    bool init() override;

    void update(float time_s, float deltaTime_s) override;

    void renderUI() override;

    void render(
        float time_s,
        int screenWidth,
        int screenHeight,
        eeng::ForwardRendererPtr renderer) override;

    void destroy() override;
};

#endif