
#include <fstream>
#include <string>
#include <sstream>

#include "ForwardRenderer.hpp"
#include "glcommon.h"
#include "ShaderLoader.h"
#include "Log.hpp"

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

    GLuint create_checker_texture()
    {
        unsigned char texture_data[256 * 256];
        unsigned int texture_id = 0;

        int x, y, i = 0;
        for (y = 0; y < 256; y++)
        {
            for (x = 0; x < 256; x++)
            {
                int k = ((x >> 4) & 1) ^ ((y >> 4) & 1);
                texture_data[i++] = k ? 255 : 192;
                //            texture_data[i++] = 125;
            }
        }

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        //.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_GREEN /**/, GL_UNSIGNED_BYTE, texture_data);
        //.
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckAndThrowGLErrors();

        return texture_id;
    }
}

namespace eeng
{
    ForwardRenderer::ForwardRenderer()
    {
    }

    ForwardRenderer::~ForwardRenderer()
    {
        EENG_ASSERT(phongShader, "Destrying uninitialized shader program");
        if (phongShader)
            glDeleteProgram(phongShader);

        // glDeleteTextures(1, &placeholder_texture);
    }

    void ForwardRenderer::init(const std::string &vertShaderPath,
                               const std::string &fragShaderPath)
    {
        Log::log("Compiling shaders %s, %s",
                 vertShaderPath.c_str(),
                 fragShaderPath.c_str());
        // UILog::log((std::string("Compiling shaders ") + vertShaderPath + std::string(", ") + fragShaderPath).c_str());
        auto vertSource = file_to_string(vertShaderPath);
        auto fragSource = file_to_string(fragShaderPath);
        phongShader = createShaderProgram(vertSource.c_str(), fragSource.c_str());

        // Bind shader samplers to texture units
        glUseProgram(phongShader);
        for (auto &textureDesc : texturesDescs)
        {
            glUniform1i(glGetUniformLocation(phongShader, textureDesc.samplerName), textureDesc.textureUnit);
        }
        // glUniform1i(glGetUniformLocation(phongShader, cubemapDesc.samplerName), cubemapDesc.textureUnit);

        // glUniform1i(glGetUniformLocation(phongShader, diffuseTextureName), diffuseTextureUnit);
        // glUniform1i(glGetUniformLocation(phongShader, normalTextureName), normalTextureUnit);
        // glUniform1i(glGetUniformLocation(phongShader, specularTextureName), specularTextureUnit);
        // glUniform1i(glGetUniformLocation(phongShader, opacityTextureName), opacityTextureUnit);
        // glUniform1i(glGetUniformLocation(phongShader, cubeTextureName), cubeTextureUnit);
        glUseProgram(0);
        CheckAndThrowGLErrors();

        // placeholder_texture = create_checker_texture();
    }

    void ForwardRenderer::beginPass(const m4f &ProjMatrix,
                                    const m4f &ViewMatrix,
                                    const v3f &lightPos,
                                    const v3f &lightColor,
                                    const v3f &eyePos)
    {
        EENG_ASSERT(phongShader, "Renderer not initialized");

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
        // glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        // glClearDepth(1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        glUseProgram(phongShader);

        // Bind matrices
        const auto ProjViewMatrix = ProjMatrix * ViewMatrix;
        glUniformMatrix4fv(glGetUniformLocation(phongShader, "ProjViewMatrix"), 1, 0, ProjViewMatrix.array);

        // Bind light & eye position
        glUniform3fv(glGetUniformLocation(phongShader, "lightpos"), 1, lightPos.vec);
        glUniform3fv(glGetUniformLocation(phongShader, "lightColor"), 1, lightColor.vec);
        glUniform3fv(glGetUniformLocation(phongShader, "eyepos"), 1, eyePos.vec);

        // Bind cube map texture
        GLuint cubemapTextureHandle = 0; // <- PLACEHOLDER
        if (cubemapTextureHandle)
        {
            // const auto &cubemapTextureDesc = texturesDescs[TextureTypeIndex::Cubemap];
            glActiveTexture(GL_TEXTURE0 + cubemapTextureDesc.textureUnit);
            glBindTexture(GL_TEXTURE_2D, cubemapTextureHandle);

            glUniform1i(glGetUniformLocation(phongShader, cubemapTextureDesc.flagName), 1);
        }

        CheckAndThrowGLErrors();
        drawcallCounter = 0;
    }

    int ForwardRenderer::endPass()
    {
        glUseProgram(0);
        glBindVertexArray(0);

        // Possibly restore GL state

        return drawcallCounter;
    }

    void ForwardRenderer::renderMesh(const std::shared_ptr<RenderableMesh> mesh,
                                     const m4f &WorldMatrix)
    {
        // Bind bone matrices
        if (mesh->boneMatrices.size())
            glUniformMatrix4fv(glGetUniformLocation(phongShader, "BoneMatrices"),
                               (GLsizei)mesh->boneMatrices.size(),
                               0,
                               mesh->boneMatrices[0].array);

        glBindVertexArray(mesh->m_VAO);

        for (uint i = 0; i < mesh->m_meshes.size(); i++)
        {
            const auto &submesh = mesh->m_meshes[i];
            const auto &mtl = mesh->m_materials[submesh.mtl_index];

            // Append hierarchical transform non-skinned meshes that are linked to nodes
            if (submesh.node_index != EENG_NULL_INDEX && !submesh.is_skinned)
            {
                const m4f WorldMeshMatrix = WorldMatrix * mesh->m_nodetree.nodes[submesh.node_index].global_tfm;
                glUniformMatrix4fv(glGetUniformLocation(phongShader, "WorldMatrix"), 1, 0, WorldMeshMatrix.array);
            }
            else
                glUniformMatrix4fv(glGetUniformLocation(phongShader, "WorldMatrix"), 1, 0, WorldMatrix.array);

            // (Could do view frustum culling (VFC) here using the projection matrix)
            // (Mesh traversal)
            // if (submesh.is_skinned)
            //     submesh.aabb = meshres.src_mesh->m_model_aabb;
            // else
            //     submesh.aabb = meshres.src_mesh->m_mesh_aabbs_pose[i];
            // (VFC)
            // v4f bs = aabb.post_transform(tfm).get_boundingsphere();

            // Color components
            glUniform3fv(glGetUniformLocation(phongShader, "Ka"), 1, mtl.Ka.vec);
            glUniform3fv(glGetUniformLocation(phongShader, "Kd"), 1, mtl.Kd.vec);
            glUniform3fv(glGetUniformLocation(phongShader, "Ks"), 1, mtl.Ks.vec);
            glUniform1f(glGetUniformLocation(phongShader, "shininess"), mtl.shininess);

            // Bind textures and texture flags
            for (auto &textureDesc : texturesDescs)
            {
                // if (texture.textureTypeIndex == TextureTypeIndex::Cubemap) continue;
                const int textureIndex = mtl.textureIndices[textureDesc.textureTypeIndex];
                const bool hasTexture = (textureIndex != NO_TEXTURE);
                if (hasTexture)
                {
                    glActiveTexture(GL_TEXTURE0 + textureDesc.textureUnit);
                    glBindTexture(GL_TEXTURE_2D, mesh->m_textures[textureIndex].getHandle());
                }
                glUniform1i(glGetUniformLocation(phongShader, textureDesc.flagName), hasTexture);
            }

            // Skinned flag
            glUniform1i(glGetUniformLocation(phongShader, "u_is_skinned"), (int)submesh.is_skinned);

            // Render
            glDrawElementsBaseVertex(GL_TRIANGLES,
                                     submesh.nbr_indices,
                                     GL_UNSIGNED_INT,
                                     (GLvoid *)(sizeof(uint) * submesh.base_index),
                                     submesh.base_vertex);
            drawcallCounter++;

                // Unbind textures
                for (auto &texture : texturesDescs)
            {
                glActiveTexture(GL_TEXTURE0 + texture.textureUnit);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            CheckAndThrowGLErrors();
        }

        glBindVertexArray(0);
    }

} // namespace eeng