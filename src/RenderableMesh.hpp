//
//  assimp_mesh.hpp
//  glfwassimp
//
//  Created by Carl Johan Gribel on 2018-02-21.
//  Copyright © 2018 Carl Johan Gribel. All rights reserved.
//

#ifndef assimp_mesh_hpp
#define assimp_mesh_hpp
// std
#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <string> // std::to_string
// GL
#include "glcommon.h"
//#include "GL/glew.h"
//#include <GLFW/glfw3.h>
// #ifdef __APPLE__
// #include <OpenGL/gl.h>
// #else
// #include <windows.h>
// #include <GL/gl.h>
// #endif

// Assimp
//#include <assimp/cimport.h>
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
// (logging & debugging)
#include "logstreamer.h"
//#include "debug_renderer.h"
//#include "glyph_renderer.hpp"

using namespace linalg;
//using linalg::v2f;
//using linalg::v3f;
//using linalg::m3f;
//using linalg::m4f;
//using linalg::quatf;
//using linalg::m4f_1;
using namespace logstreamer;
//using namespace gl_batch_renderer;
//class glyph_renderer_t;

// Padding keyframes to nodes will eliminate the need for a hash between them,
// potentially leading to better play performance at the cost of extra space
#define PadKeyframesToNodes

const int NUM_BONES_PER_VERTEX = 4;
const int NO_MATERIAL = -1;
const int NO_TEXTURE = -1;

template<std::size_t N, class T>
constexpr std::size_t numelem(T(&)[N]) { return N; }

struct xiDefaultMaterial
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
};

enum xiContentFlags
{
    xi_load_meshes = 0x1,
    xi_load_animations = 0x2
};

class RenderableMesh
{
//    friend class AnimController;
//    friend class ClipNode;
    
private:
    
    enum {
        INDEX_BUFFER,
        POS_VB,
        NORMAL_VB,
        TANGENT_VB,
        BINORMAL_VB,
        TEXCOORD_VB,
        BONE_VB,
        NUM_VBs
    };
    
    struct mesh_t
    {
        unsigned base_index = 0;
        unsigned nbr_indices = 0;
        unsigned base_vertex = 0;
        unsigned nbr_vertices = 0;
        
        int mtl_index = -1;
        int node_index = -1; // EXPERIMENTAL
        bool is_skinned = false;
    };
    
    struct bone_t
    {
        m4f inversebind_tfm = m4f_1;   // Inverse bind-pose transform
        m4f global_tfm = m4f_1;  // Built during traversal * reduntant, use bone array directly for final bone transforms *
        int node_index = -1; // EXPERIMENTAL
//        AABB_t aabb;
    };
    // This is probably how things should look for bones:
    // std::vector<m4f> bone_array -> in AnimationController
    // std::vector<m4f> bone_inversebinds
    // std::vector<aabb_t> bone_aabbs
    //
    // And something like this for nodes (pull out from seqtree_t::node_t):
    // std::vector<m4f> localTfm // mTransformation
    // std::vector<m4f> globalTfm // travsersal
    // + name strings and perhaps other node stuff
    
    struct vertex_skindata_t
    {
        unsigned bone_indices[NUM_BONES_PER_VERTEX] = {0};
        float bone_weights[NUM_BONES_PER_VERTEX] = {0};
        
        int nbr_added = 0; // For checking
        void add_weight(unsigned bone_index, float bone_weight);
    };
    
    // Keyframe sequence for a particular node and animation
    // The number of scaling/rotation/translation channels
    // are not necessarily equal.
    struct node_animation_t
    {
        std::string name; // node_name
        std::vector<v3f> pos_keys;
        std::vector<v3f> scale_keys;
        std::vector<quatf> rot_keys;
    };
    
    struct animation_t
    {
        std::string name;
        float duration_ticks = 0;
        float tps = 1;
        std::vector<node_animation_t> node_animations;
#ifndef PadKeyframesToNodes
        std::unordered_map<std::string, unsigned> node_animation_hash;
#endif
    };
    
    // GL stuff
    
    GLuint m_VAO = 0;
    GLuint m_Buffers[NUM_VBs] = {0};
    GLuint default_shader;
    GLuint placeholder_texture;
    //GLuint test_texture;
    
    // Assimp stuff
    // Todo: import what's needed and then release these
    
//    const aiScene* m_pScene;
//    Assimp::Importer m_importer; // owns the loaded data (pointed to by aiScene*)

    // Scene stuff
    
public:
    m4f M_global, M_global_inverse;

    // Node hierarchy
    seqtree_t<SkeletonNode>     m_nodetree;
    std::vector<bone_t>         m_bones;
    // Geometry & materials
    std::vector<mesh_t>         m_meshes;
    std::vector<xiDefaultMaterial>  m_materials;
    std::vector<Texture2D> m_textures;
    // Bounding volumes
    std::vector<AABB_t>         m_bone_aabbs_bind;      // Per-bone bind AABB
    std::vector<AABB_t>         m_bone_aabbs_pose;      // Per-node pose AABB's – intermediary, used for visualization
    std::vector<AABB_t>         m_mesh_aabbs_bind;      // Per-mesh bind AABB
    std::vector<AABB_t>         m_mesh_aabbs_pose;      // Per-mesh pose AABB's – intermediary, used for visualization
    AABB_t                      m_model_aabb;           // AABB for the entire model
    // Animations
    std::vector<animation_t>    m_animations;
    
    // DEV
////    std::vector<v3f> points;
////    AABB_t points_AABB;
//    // ANIM DEV
//    // bi-blend
//    float tickA = 0, tickB = 0;
//    float animfrac = 0;
//    // quad blend
//    float tick0 = 0;
//    float l, m;
//    float b0, b1, b2, b3;
////    float fracA = 0, fracB = 0;
//    std::vector<m4f> DEV_additiveTfm;
//    std::vector<m4f> DEV_additiveTfmExport;
////    std::vector<m4f> DEV_renderBoneArray;
//    //
//    int current_anim_tick = 0;
//    int current_anim_totticks = 0;
//    float current_anim_frac = 0;
    
//private:
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
    
    void load(const std::string& file,
              bool just_animations = false);
    void load(const std::string& file,
                             unsigned xiflags,
                             unsigned aiflags = 0);
    
    void animate(int anim_index,
                 float time,
                 std::vector<m4f>& bone_transforms);
    
    void render(const m4f& PROJ_VIEW,
                const m4f& WORLD,
                double time,
                int anim_index,
                const v3f& lightpos,
                const v3f& eyepos,
                gl_cubemap_t* cubemap = nullptr,
                GLuint shader = 0);
    
    void render(const m4f& PROJ_VIEW,
                const m4f& WORLD,
                const std::vector<m4f>& bone_array,
                const v3f& lightpos,
                const v3f& eyepos,
                gl_cubemap_t* cubemap = nullptr,
                GLuint shader = 0);
    
    unsigned get_nbr_animations() const;
    std::string get_animation_name(unsigned i) const;

/*
    // These aux render methods should be lifted out of the class
    //
    void render_nodes(gl_batch_renderer::glDebugBatchRenderer* dbgrenderer,
                      const m4f& M,
                      bool render_basis_arrows = false,
                      float basis_arrow_scale = 10) const;
                      */
    //void render_node_tags(glyph_renderer_t* glyphrenderer) const;


private:

    bool load_scene(const aiScene* pScene,
                    const std::string& file);
    void load_mesh(uint MeshIndex,
                   const aiMesh* paiMesh,
                   std::vector<v3f>& Positions,
                   std::vector<v3f>& Normals,
                   std::vector<v3f>& Tangents,
                   std::vector<v3f>& Binormals,
                   std::vector<v2f>& TexCoords,
                   std::vector<vertex_skindata_t>& Bones,
                   std::vector<unsigned int>& Indices);
    
    void compute_bind_aabbs();  // not implemented. where?
    void compute_pose_aabbs();  // not implemented. where?
    
    void load_nodes(aiNode* node);
    void load_node(aiNode* node);
    
    /// Load bones and skin weights associated with a mesh
    ///
    void load_bones(uint mesh_index,
                    const aiMesh* aimesh,
                    std::vector<vertex_skindata_t>& scene_skindata);
    
    void load_materials(const aiScene* aiscene,
                        const std::string& file);
    
    int load_texture(const aiMaterial* aimtl,
                     aiTextureType tex_type,
                     const std::string& local_filepath);
    
    void load_animations(const aiScene* scene);
    m4f blend_transform_at_time(const animation_t* anim,
                                const node_animation_t& nodeanim,
                                float time) const;
    m4f blend_transform_at_frac(const animation_t* anim,
                                const node_animation_t& nodeanim,
                                float frac) const;
    
    AABB_t measure_scene(const aiScene* aiscene);
    void measure_node(const aiScene* aiscene,
                      const aiNode* pNode,
                      const m4f& transform,
                      AABB_t& aabb);
    void measure_mesh(const aiMesh* pMesh,
                      const m4f& transform,
                      AABB_t& aabb);
    
public:
    

    //
    // DEV weight generation
    //
    // -> Skeleton class
    //
    
    void TEST_set_weights_gradient(const v3f& n,
                                   const std::string& pivot_node,
                                   const m4f& worldMx,
                                   std::vector<float>& weights)
    {
        auto nit = m_nodehash.find(pivot_node);
        if (nit == m_nodehash.end()) return;
        int pivot_index = nit->second;
        v3f pivot_pos = extract_translation( worldMx * m_nodetree.nodes[pivot_index].global_tfm );
        
        std::vector<float> node_distances( weights.size() );
        float model_min = 1e3, model_max = -1e3, model_length;
        
        for (int i=0; i < m_nodetree.nodes.size(); i++)
        {
            v3f node_pos = extract_translation( worldMx * m_nodetree.nodes[i].global_tfm );
            v3f nvec = node_pos - pivot_pos;
            float d = nvec.dot(n);
            node_distances[i] = d;
            model_min = fmin(model_min, d);
            model_max = fmax(model_max, d);
            weights[i] = linalg::smoothstep(d, -20.0f, 20.0f);
        }
        model_length = fabs(model_max - model_min);
        
        // If normalized gradient: iterate again and divide deist's by model_lengths
        
        //float dnorm = (d + model_min)/model_length;
    }
    
    void TEST_set_weights_branch(const std::string& pivot_node,
                                 float weight,
                                 std::vector<float>& weights)
    {
        auto nit = m_nodehash.find(pivot_node);
        if (nit == m_nodehash.end()) return;
        int pivot_index = nit->second;
        
        weights[pivot_index] = weight;
        int index = pivot_index;
        while ( index < m_nodetree.nodes.size() ) // + abort at succeeding tree
        {
            int cindex = index+1;
            for (int i=0; i<m_nodetree.nodes[index].m_nbr_children; i++)
            {
                weights[cindex] = weight;
                cindex += m_nodetree.nodes[cindex].m_branch_stride;
            }
            index++;
        }
    }
    
    inline vec3f color_heatmap(const float &x)
    {
        if (x < 0.25f)    return vec3f( 0, lerp<float>(0, 1, x*4), 1);
        if (x < 0.5f)    return vec3f( 0, 1 , lerp<float>(1, 0, (x-0.25f)*4));
        if (x < 0.75f)    return vec3f( lerp<float>(0, 1, (x-0.5f)*4), 1, 0);
        return vec3f(1, lerp<float>(1, 0, (x-0.75f)*4) , 0 );
    }
    
    /*
    void TEST_render_weights(gl_batch_renderer::glDebugBatchRenderer* dbgrenderer,
                             glyph_renderer_t* glyphrenderer,
                             const m4f& worldMx,
                             std::vector<float>& weights)
    {
        for (int i=0; i < m_nodetree.nodes.size(); i++)
        {
            v3f node_pos = extract_translation( worldMx * m_nodetree.nodes[i].global_tfm );
            v3f color = color_heatmap(weights[i]); // v3f(0,1,0) * weights[i];
            dbgrenderer->push_point(node_pos, color, 8);
            glyphrenderer->add_paragraph_at(std::to_string(weights[i]),
                                            node_pos,
                                            false,
                                            {0,0,0},
                                            {1,1,1,1});
        }
    }
    */
    
    /*
    void TEST_upward_traverse(gl_batch_renderer::glDebugBatchRenderer* dbgrenderer,
                              const m4f& worldMx,
                              const std::string& node_name)
    {
        auto nit = m_nodehash.find(node_name);
        if (nit == m_nodehash.end()) return;
        int index = nit->second;
        
        while ( m_nodetree.nodes[index].m_parent_ofs )
        {
            //v3f p = extract_translation( worldMx * m_nodetree.nodes[index].global_tfm );
            dbgrenderer->push_sphere(2, 2, worldMx * m_nodetree.nodes[index].global_tfm, DBGCOLOR::BLUE);
            index -= m_nodetree.nodes[index].m_parent_ofs;
        }
    }
    */
    
    //
    // /DEV weight generation
    //
};

#endif /* assimp_mesh_hpp */
