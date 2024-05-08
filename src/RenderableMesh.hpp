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
// lib
#include "vec.h"
#include "mat.h"
#include "quat.h"
#include "interp.h" // smoothstep
#include "AABB.h"
#include "Texture.hpp"
#include "VectorTree.h"
#include "logstreamer.h"

namespace eeng
{
    using namespace linalg;
    using namespace logstreamer;
    using uint = uint32_t;

    const int NUM_BONES_PER_VERTEX = 4;
    const int NO_MATERIAL = -1;
    const int NO_TEXTURE = -1;

    template <std::size_t N, class T>
    constexpr std::size_t numelem(T (&)[N]) { return N; }

    struct PhongMaterial
    {
        v3f Ka = {0.25f, 0.0f, 0.0f};
        v3f Kd = {0.75f, 0.0f, 0.0f};
        v3f Ks = {1.0f, 1.0f, 1.0f};
        float shininess = 10;

        int diffuse_texture_index = NO_TEXTURE;
        int normal_texture_index = NO_TEXTURE;
        int specular_texture_index = NO_TEXTURE;
        int opacity_texture_index = NO_TEXTURE;
        int reflective_texture_index = NO_TEXTURE;

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

        /**
         *
         */
        struct Submesh
        {
            unsigned base_index = 0;
            unsigned nbr_indices = 0;
            unsigned base_vertex = 0;
            unsigned nbr_vertices = 0;

            int mtl_index = -1;
            int node_index = -1; // EXPERIMENTAL
            bool is_skinned = false;
        };

        /**
         * Per-bone data
         */
        struct Bone
        {
            m4f inversebind_tfm = m4f_1; //!< Inverse of the associated node in bind pose
            // m4f global_tfm = m4f_1;      // Built during traversal * reduntant, use bone array directly for final bone transforms *
            int node_index = -1; //!< Node associated with this bone
        };

        /**
         * Bone indices and weights for a vertex
         */
        struct SkinData
        {
            unsigned bone_indices[NUM_BONES_PER_VERTEX]{0};
            float bone_weights[NUM_BONES_PER_VERTEX]{0};

            int nbr_added = 0; // For checking
            void add_weight(unsigned bone_index, float bone_weight);
        };

        /**
         * Keyframe sequence for a node and an animation.
         */
        struct NodeKeyframes // NodeKeyframes ???
        {
            bool is_used = false;
            std::vector<v3f> pos_keys;
            std::vector<v3f> scale_keys;
            std::vector<quatf> rot_keys;
        };

        /**
         * Data related to an animation clip, including keyframes for all nodes.
         */
        struct AnimationClip
        {
            std::string name;
            float duration_ticks = 0;
            float tps = 1;
            std::vector<NodeKeyframes> node_animations;
        };

        // GL stuff

        GLuint m_VAO = 0;
        GLuint m_Buffers[BufferCount] = {0};
        GLuint default_shader;
        GLuint placeholder_texture;
        // GLuint test_texture;

        // Assimp stuff
        // Todo: import what's needed and then release these

        //    const aiScene* m_pScene;
        //    Assimp::Importer m_importer; // owns the loaded data (pointed to by aiScene*)

        // Scene stuff

    public:
        // m4f M_global, M_global_inverse;

        // Node hierarchy
        VectorTree<SkeletonNode> m_nodetree;
        std::vector<Bone> m_bones;
        std::vector<m4f> boneMatrices;
        // Geometry & materials
        std::vector<Submesh> m_meshes;
        std::vector<PhongMaterial> m_materials;
        std::vector<Texture2D> m_textures;
        // Bounding volumes
        std::vector<AABB_t> m_bone_aabbs_bind; // Per-bone bind AABB
        std::vector<AABB_t> m_bone_aabbs_pose; // Per-node pose AABB's – intermediary, used for visualization
        std::vector<AABB_t> m_mesh_aabbs_bind; // Per-mesh bind AABB
        std::vector<AABB_t> m_mesh_aabbs_pose; // Per-mesh pose AABB's – intermediary, used for visualization
        AABB_t m_model_aabb;                   // AABB for the entire model
        // Animations
        std::vector<AnimationClip> m_animations;

    public:
        unsigned m_embedded_textures_ofs = 0;

        using index_hash_t = std::unordered_map<std::string, unsigned>;
        index_hash_t m_texturehash; // full file path, or just filename for embedded textures
        index_hash_t m_bonehash;
        index_hash_t m_nodehash;

        // Log & debug stuff

        logstreamer_t log;
        //    gl_batch_renderer::glDebugBatchRenderer* dbgrenderer = nullptr;

        //    template<typename T>
        //    inline T lerp(const T &a, const T &b, float x) { return a*(1.0f-x) + b*x; }

    public:
        AABB_t mSceneAABB;

        RenderableMesh();

        ~RenderableMesh();

        void load(const std::string &file,
                  bool just_animations = false);
        void load(const std::string &file,
                  unsigned xiflags,
                  unsigned aiflags = 0);

        void remove_translation_keys(const std::string &node_name);

        void remove_translation_keys(int node_index);

        void animate(int anim_index,
                     float time);
        // std::vector<m4f> &bone_transforms);

        void render(const m4f &PROJ_VIEW,
                    const m4f &WORLD,
                    double time,
                    int anim_index,
                    const v3f &lightpos,
                    const v3f &eyepos,
                    gl_cubemap_t *cubemap = nullptr,
                    GLuint shader = 0);

        void render(const m4f &PROJ_VIEW,
                    const m4f &WORLD,
                    // const std::vector<m4f> &bone_array,
                    const v3f &lightpos,
                    const v3f &eyepos,
                    gl_cubemap_t *cubemap = nullptr,
                    GLuint shader = 0);

        unsigned get_nbr_animations() const;
        std::string get_animation_name(unsigned i) const;

    private:
        bool load_scene(const aiScene *pScene,
                        const std::string &file);
        void load_mesh(uint MeshIndex,
                       const aiMesh *paiMesh,
                       std::vector<v3f> &Positions,
                       std::vector<v3f> &Normals,
                       std::vector<v3f> &Tangents,
                       std::vector<v3f> &Binormals,
                       std::vector<v2f> &TexCoords,
                       std::vector<SkinData> &Bones,
                       std::vector<unsigned int> &Indices);

        void compute_bind_aabbs(); // not implemented. where?
        void compute_pose_aabbs(); // not implemented. where?

        void load_nodes(aiNode *node);
        void load_node(aiNode *node);

        /// Load bones and skin weights associated with a mesh
        ///
        void load_bones(uint mesh_index,
                        const aiMesh *aimesh,
                        std::vector<SkinData> &scene_skindata);

        void load_materials(const aiScene *aiscene,
                            const std::string &file);

        int load_texture(const aiMaterial *aimtl,
                         aiTextureType tex_type,
                         const std::string &local_filepath);

        void load_animations(const aiScene *scene);
        m4f blend_transform_at_time(const AnimationClip *anim,
                                    const NodeKeyframes &nodeanim,
                                    float time) const;
        m4f blend_transform_at_frac(const AnimationClip *anim,
                                    const NodeKeyframes &nodeanim,
                                    float frac) const;

        AABB_t measure_scene(const aiScene *aiscene);
        void measure_node(const aiScene *aiscene,
                          const aiNode *pNode,
                          const m4f &transform,
                          AABB_t &aabb);
        void measure_mesh(const aiMesh *pMesh,
                          const m4f &transform,
                          AABB_t &aabb);
    };

} /* namespace eeng */

#endif /* RenderableMesh_hpp */
