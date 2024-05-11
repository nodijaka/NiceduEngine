//
//  assimp_mesh.hpp
//  glfwassimp
//
//  Created by Carl Johan Gribel on 2018-02-21.
//  Copyright © 2018 Carl Johan Gribel. All rights reserved.
//

#ifndef RenderableMesh_hpp
#define RenderableMesh_hpp
// std
#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <string>
// GL
#include "glcommon.h"
// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

// lib
// #include "vec.h"
// #include "mat.h"
// #include "quat.h"
// #include "interp.h" // smoothstep
#include "AABB.h"
#include "Texture.hpp"
#include "VectorTree.h"
#include "logstreamer.h"

namespace eeng
{
    // using namespace linalg;
    using namespace logstreamer;
    using uint = uint32_t;

    const int NUM_BONES_PER_VERTEX = 4;
    const int NO_MATERIAL = -1;
    const int NO_TEXTURE = -1;

    template <std::size_t N, class T>
    constexpr std::size_t numelem(T (&)[N]) { return N; }

    /// A material with typical Phong illumination properties
    struct PhongMaterial
    {
        glm::vec3 Ka = {0.25f, 0.0f, 0.0f};
        glm::vec3 Kd = {0.75f, 0.0f, 0.0f};
        glm::vec3 Ks = {1.0f, 1.0f, 1.0f};
        float shininess = 10;

        enum TextureTypeIndex
        {
            Diffuse = 0,
            Normal,
            Specular,
            Opacity,
            Cubemap,
            Count
        };
        int textureIndices[TextureTypeIndex::Count]{NO_TEXTURE};
    };

    enum xiContentFlags
    {
        xi_load_meshes = 0x1,
        xi_load_animations = 0x2
    };

    /// @brief A model loaded from file prepared with GL textures and buffers
    class RenderableMesh
    {
        friend class ForwardRenderer;

    private:
        enum
        {
            IndexBuffer,
            PositionBuffer,
            NormalBuffer,
            TangentBuffer,
            BinormalBuffer,
            TexturecoordBuffer,
            BoneBuffer,
            BufferCount
        };

        /// A mesh with a single material and a given set of geometry
        struct Submesh
        {
            // TODO: GL types?
            unsigned base_index = 0;
            unsigned nbr_indices = 0;
            unsigned base_vertex = 0;
            unsigned nbr_vertices = 0;

            int mtl_index = -1;
            int node_index = -1;
            bool is_skinned = false;
        };

        /// Per-bone data
        struct Bone
        {
            glm::mat4 inversebind_tfm{1.0f}; //!< Inverse of the associated node in bind pose
            int node_index = -1;             //!< Node associated with this bone
        };

        /// Bone indices and weights for a vertex
        struct SkinData
        {
            unsigned bone_indices[NUM_BONES_PER_VERTEX]{0};
            float bone_weights[NUM_BONES_PER_VERTEX]{0};

            int nbr_added = 0; // For checking
            void addWeight(unsigned bone_index, float bone_weight);
        };

        /// Keyframe sequence for a node and an animation.
        struct NodeKeyframes // NodeKeyframes ???
        {
            bool is_used = false;
            std::vector<glm::vec3> pos_keys;
            std::vector<glm::vec3> scale_keys;
            std::vector<glm::quat> rot_keys;
        };

        /// Data related to an animation clip, including keyframes for all nodes.
        struct AnimationClip
        {
            std::string name;
            float duration_ticks = 0;
            float tps = 1;
            std::vector<NodeKeyframes> node_animations;
        };

        GLuint m_VAO = 0;
        GLuint m_Buffers[BufferCount] = {0};

    public:
        VectorTree<SkeletonNode> m_nodetree;
        std::vector<Bone> m_bones;
        std::vector<glm::mat4> boneMatrices;
        std::vector<AnimationClip> m_animations;

        std::vector<Submesh> m_meshes;
        std::vector<PhongMaterial> m_materials;
        std::vector<Texture2D> m_textures;

        // Bounding volumes
        std::vector<AABB> m_bone_aabbs_bind; // Per-bone bind AABB
        std::vector<AABB> m_bone_aabbs_pose; // Per-node pose AABB's – intermediary, used for visualization
        std::vector<AABB> m_mesh_aabbs_bind; // Per-mesh bind AABB
        std::vector<AABB> m_mesh_aabbs_pose; // Per-mesh pose AABB's – intermediary, used for visualization
        AABB m_model_aabb;                   // AABB for the entire model

    public:
        unsigned m_embedded_textures_ofs = 0;

        using index_hash_t = std::unordered_map<std::string, unsigned>;
        index_hash_t m_texturehash; // full file path, or just filename for embedded textures
        index_hash_t m_bonehash;
        index_hash_t m_nodehash;

        // Log & debug stuff
        logstreamer_t log;

    public:
        AABB mSceneAABB;

        RenderableMesh();

        ~RenderableMesh();

        void load(const std::string &file,
                  bool just_animations = false);
        void load(const std::string &file,
                  unsigned xiflags,
                  unsigned aiflags = 0);

        void removeTranslationKeys(const std::string &node_name);

        void removeTranslationKeys(int node_index);

        void animate(int anim_index,
                     float time);

        unsigned getNbrAnimations() const;
        std::string getAnimationName(unsigned i) const;

    private:
        bool loadScene(const aiScene *pScene,
                       const std::string &file);
        void loadMesh(uint MeshIndex,
                      const aiMesh *paiMesh,
                      std::vector<glm::vec3> &Positions,
                      std::vector<glm::vec3> &Normals,
                      std::vector<glm::vec3> &Tangents,
                      std::vector<glm::vec3> &Binormals,
                      std::vector<glm::vec2> &TexCoords,
                      std::vector<SkinData> &Bones,
                      std::vector<unsigned int> &Indices);

        void compute_bind_aabbs(); // not implemented. where?
        void compute_pose_aabbs(); // not implemented. where?

        void loadNodes(aiNode *node);
        void loadNode(aiNode *node);

        /// Load bones and skin weights associated with a mesh
        ///
        void loadBones(uint mesh_index,
                       const aiMesh *aimesh,
                       std::vector<SkinData> &scene_skindata);

        void loadMaterials(const aiScene *aiscene,
                           const std::string &file);

        int loadTexture(const aiMaterial *aimtl,
                        aiTextureType tex_type,
                        const std::string &local_filepath);

        void loadAnimations(const aiScene *scene);

        glm::mat4 blendTransformAtTime(const AnimationClip *anim,
                                       const NodeKeyframes &nodeanim,
                                       float time) const;

        glm::mat4 blendTransformAtFrac(const AnimationClip *anim,
                                       const NodeKeyframes &nodeanim,
                                       float frac) const;

        AABB measureScene(const aiScene *aiscene);

        void measureNode(const aiScene *aiscene,
                         const aiNode *pNode,
                         const glm::mat4 &transform,
                         AABB &aabb);

        void measureMesh(const aiMesh *pMesh,
                         const glm::mat4 &transform,
                         AABB &aabb);
    };

} /* namespace eeng */

#endif /* RenderableMesh_hpp */
