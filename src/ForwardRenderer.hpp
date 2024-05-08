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

        //     const char *diffuseTextureName = "diffuseTexture";
        // const char *normalTextureName = "normalTexture";
        // const char *specularTextureName = "specularTexture";
        // const char *opacityTextureName = "opacityTexture";
        // const char *cubeTextureName = "cubeTexture";

        // const GLuint diffuseTextureUnit = 0;
        // const GLuint normalTextureUnit = 1;
        // const GLuint specularTextureUnit = 2;
        // const GLuint opacityTextureUnit = 3;
        // const GLuint cubeTextureUnit = 4;

    public:
        ForwardRenderer();

        ~ForwardRenderer();

        void init(const std::string &vertShaderPath,
                  const std::string &fragShaderPath);

        void beginPass(const m4f &ProjMatrix,
                       const m4f &ViewMatrix,
                       const v3f &lightPos,
                       const v3f &eyePos);

        void endPass();

        // Bone array ???

        void renderMesh(const std::shared_ptr<RenderableMesh> mesh,
                        const m4f &WorldMatrix);

    private:
        // void bindTexture(const RenderableMesh &mesh, int textureIndex)
        // {
        //             const bool use_default_diffuse = false;
        // bool has_diffusetex = (m_materials[m_meshes[i].mtl_index].diffuse_texture_index != NO_TEXTURE);
        // if (has_diffusetex)
        //     m_textures[m_materials[m_meshes[i].mtl_index].diffuse_texture_index].bind(GL_TEXTURE0);
        // else if (use_default_diffuse)
        // {
        //     glActiveTexture(GL_TEXTURE0);
        //     glBindTexture(GL_TEXTURE_2D, placeholder_texture);
        //     has_diffusetex = true;
        // }
        // glUniform1i(glGetUniformLocation(shader, "has_diffusetex"), (int)has_diffusetex);
        // }
    };

} // namespace eeng

#endif