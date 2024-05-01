#ifndef FORWARDRENDERER_HPP
#define FORWARDRENDERER_HPP
// std
// #include <iostream>
// #include <stdio.h>
// #include <vector>
// #include <map>
// #include <unordered_map>
// #include <fstream>
// #include <string>
// GL
#include "glcommon.h"
#include "RenderableMesh.hpp"
// lib
#include "vec.h"
#include "mat.h"
// #include "quat.h"
// #include "interp.h" // smoothstep
// #include "AABB.h"
// #include "Texture.hpp"
// #include "VectorTree.h"
// #include "logstreamer.h"

class ForwardRenderer
{
    // GLboolean depthMask = GL_TRUE;
    GLuint phong_shader = 0;

public:
    ForwardRenderer()
    {
    }

    void beginPass()
    {
        // GL state

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
        // TODO glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

        // Bind the default framebuffer (only needed when using multiple render targets)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Clear depth and color attachments of frame buffer
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // if (WIREFRAME)
        // {
        //     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //     glDisable(GL_CULL_FACE);
        // }
        // else
        // {
        //     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //     glEnable(GL_CULL_FACE);
        // }
    }

    void endPass()
    {
    }

    // Bone array ???

    void renderMesh(
        const RenderableMesh &mesh,
        const m4f &ProjectionMatrix,
        const m4f &ViewMatrix,
        const m4f &WorldMatrix)
    {
        // VFC

        // Render mesh
    }
};

#endif