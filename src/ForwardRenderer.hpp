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

namespace eeng
{

    class ForwardRenderer
    {
        GLuint phongShader = 0;
        GLuint placeholder_texture = 0;
        int drawcallCounter;

        struct TextureDesc
        {
            PhongMaterial::TextureTypeIndex textureTypeIndex;
            GLuint textureUnit;
            const char *samplerName;
            const char *flagName;
        };

        enum TextureTypeIndex
        {
            Diffuse = 0,
            Normal,
            Specular,
            Opacity,
            Cubemap
        };

        TextureDesc texturesDescs[4] = {
            {PhongMaterial::TextureTypeIndex::Diffuse, 0, "diffuseTexture", "has_diffuseTexture"},
            {PhongMaterial::TextureTypeIndex::Normal, 1, "normalTexture", "has_normalTexture"},
            {PhongMaterial::TextureTypeIndex::Specular, 2, "specularTexture", "has_specularTexture"},
            {PhongMaterial::TextureTypeIndex::Opacity, 3, "opacityTexture", "has_opacityTexture"}};
        
        TextureDesc cubemapTextureDesc {PhongMaterial::TextureTypeIndex::Cubemap, 4, "cubeTexture", "has_cubemap"};

    public:
        ForwardRenderer();

        ~ForwardRenderer();

        /// @brief Initialize renderer
        /// @param vertShaderPath 
        /// @param fragShaderPath 
        void init(const std::string &vertShaderPath,
                  const std::string &fragShaderPath);

        /// @brief Start of a rendering pass and set common uniforms
        /// @param ProjMatrix 
        /// @param ViewMatrix 
        /// @param lightPos 
        /// @param lightColor 
        /// @param eyePos 
        void beginPass(const m4f &ProjMatrix,
                       const m4f &ViewMatrix,
                       const v3f &lightPos,
                       const v3f& lightColor,
                       const v3f &eyePos);

        /// @brief Ends pass and resets GL state
        /// @return Number of drawcalls made during pass
        int endPass();

        /// @brief Render an instance of a mesh
        /// @param mesh Mesh to render
        /// @param WorldMatrix Instance world transform
        void renderMesh(const std::shared_ptr<RenderableMesh> mesh,
                        const m4f &WorldMatrix);
    };

} // namespace eeng

#endif