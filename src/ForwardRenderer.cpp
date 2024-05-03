
#include <fstream>
#include <string>
#include <sstream>

#include "ForwardRenderer.hpp"
#include "glcommon.h"
#include "ShaderLoader.h"
#include "UILog.hpp"

namespace eeng {

namespace
{
    std::string file_to_string(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            // Handle error - file not found, permissions issue, etc.
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
    }
}

ForwardRenderer::ForwardRenderer()
{
}

ForwardRenderer::~ForwardRenderer()
{
    EENG_ASSERT(phong_shader, "Destrying uninitialized shader program");
    if (phong_shader)
        glDeleteProgram(phong_shader);
}

void ForwardRenderer::init(const std::string &vertShaderPath,
                           const std::string &fragShaderPath)
{
    Log::log("Compiling shaders %s, %s",
               vertShaderPath.c_str(),
               fragShaderPath.c_str());
    //UILog::log((std::string("Compiling shaders ") + vertShaderPath + std::string(", ") + fragShaderPath).c_str());
    auto vertSource = file_to_string(vertShaderPath);
    auto fragSource = file_to_string(fragShaderPath);
    phong_shader = createShaderProgram(vertSource.c_str(), fragSource.c_str());

    // placeholder_texture = create_checker_texture();
}

void ForwardRenderer::beginPass()
{
    EENG_ASSERT(phong_shader, "Renderer not initialized");

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

void ForwardRenderer::endPass()
{
    EENG_ASSERT(phong_shader, "Renderer not initialized");

    // Possibly restore GL state
}

void ForwardRenderer::renderMesh(
    const RenderableMesh &mesh,
    const m4f &ProjectionMatrix,
    const m4f &ViewMatrix,
    const m4f &WorldMatrix)
{
    EENG_ASSERT(phong_shader, "Renderer not initialized");
    glUseProgram(phong_shader);

    // VFC

    // Render mesh

    glBindVertexArray(0);
    glUseProgram(0);
}

} // namespace eeng