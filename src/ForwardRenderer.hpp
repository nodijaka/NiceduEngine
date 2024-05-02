#ifndef ForwardRenderer_hpp
#define ForwardRenderer_hpp
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

    const GLuint DiffuseTextureUnit = 0;
    const GLuint NormalTextureUnit = 1;
    const GLuint SpecularTextureUnit = 2;
    const GLuint OpacityTextureUnit = 3;
    const GLuint CubeTextureUnit = 4;

public:
    ForwardRenderer();

    ~ForwardRenderer();

    void init(const std::string &vertShaderPath,
              const std::string &fragShaderPath);

    void beginPass();

    void endPass();

    // Bone array ???

    void renderMesh(
        const RenderableMesh &mesh,
        const m4f &ProjectionMatrix,
        const m4f &ViewMatrix,
        const m4f &WorldMatrix);

private:
    void bindTexture(GLuint texture, GLenum unit)
    {
    }
};

#endif