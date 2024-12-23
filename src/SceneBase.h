#ifndef SceneBase_h
#define SceneBase_h
#pragma once

#include "ForwardRenderer.hpp"

namespace eeng {

    /// @brief TODO
    class SceneBase
    {
    public:
        virtual bool init() = 0;

        virtual void update(float time_s, float deltaTime_s) = 0;

        virtual void renderUI() = 0;

        virtual void render(
            float time_s,
            int screenWidth,
            int screenHeight,
            ForwardRendererPtr renderer) = 0;

        virtual void destroy() = 0;
    };
}

#endif