//
//  vectordag.h
//  tau3d
//
//  Created by Carl Johan Gribel on 2016-02-15.
//
//

// Pre-order sequential representation
// First child is always adjacent to its parent.
// Each node holds num_children and branch_stride.
// Parent-child chains are sequential in memory –
// seemingly most optimal for depth-first traversal.

#ifndef seqtree_h
#define seqtree_h

//#include <stack>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include "vec.h"
#include "mat.h"
#include "logstreamer.h"

using linalg::m4f;
using linalg::m4f_1;
using namespace logstreamer;
//template<class NodeType> class seqtree_t;

#if 1
struct TreeNode
{
//    template<class NodeType> friend class seqtree_t; // So classes can inherit privately
    unsigned m_nbr_children = 0;    // Node nbr of children
    unsigned m_branch_stride = 1;   // Branch size including this node
    unsigned m_parent_ofs = 0;      // Distance to parent, relative parent.
};

//std::vector<m4f> local_tfm, global_tfm;
// In component -> handle_t<m4f, std::vector<m4f> > { index, local_tfm }

struct SkeletonNode : public TreeNode
{
    m4f local_tfm;          // node transform rel. parent = mTransformation
    m4f global_tfm = m4f_1; // Updated during tree traversal
        
    int bone_index = -1;    // For debugging
    int nbr_meshes = 0;     // For debugging
    
    std::string name = "";  // String ???
    //    String name_;
    
    // For debugging / visualization
    std::string attribs;
    std::string JointName_attrib;    // For ragdolls
    std::string EntityType_attrib;   // "Door", "Switch", "GoalSector" ...
    std::string EntityID_attrib;     // "Switch1", "Door1", ...
    std::string MeshType_attrib;            // "RenderableMesh", "ColliderMesh", ...
    
    SkeletonNode() = default;
    SkeletonNode(const std::string& name,
                 const m4f& local_tfm) :
    name(name),
    local_tfm(local_tfm) { }
    
};
#endif

template<class NodeType>
class seqtree_t
{
public:
    
#if 0
    struct node_t
    {
        friend class seqtree_t;
        unsigned m_nbr_children = 0;    // Node nbr of children
        unsigned m_branch_stride = 1;   // Branch size including this node
        unsigned m_parent_ofs = 0;      // Distance to parent, relative parent.
                                        // 0 = root
        
        // Node data should probably be pulled out and indexed.
        std::string name = "";
        m4f local_tfm;          // node transform rel. parent = mTransformation
        m4f global_tfm = m4f_1; // Updated during tree traversal
        int bone_index = -1;    // For debugging
        int nbr_meshes = 0;     // For debugging
        //
        
        node_t() = default;
        
        node_t(const std::string& name,
               const m4f& local_tfm) :
        name(name),
        local_tfm(local_tfm)
        {
            
        }
    };
#endif
    
    std::vector<NodeType> nodes;
//    std::vector<int> data;
    
    seqtree_t() { }
    
#if 1
    NodeType* find_node(const std::string& node_name)
    {
        auto it = std::find_if(nodes.begin(), nodes.end(),
                                 [&node_name](const NodeType& node){ return !node.name.compare(node_name); } );
        if (it == nodes.end())
            return nullptr;
        return &(*it);
    }
#endif
    
    bool insert(NodeType node, const std::string& parent)
    {
        // No parent given - this is a root node
        if (!parent.size())
        {
            // Insert as root
            nodes.insert(nodes.begin(), node);
//            data.insert(data.begin(), 123);
            return true;
        }
        
        // Locate parent
        const std::function<bool(const NodeType&)> findparent_lambda =
        [&](const NodeType& node)
        {
            return !node.name.compare(parent);
        };
        auto pit = std::find_if(nodes.begin(), nodes.end(), findparent_lambda);

        // Parent not found - corrupt hierarchy
//        assert(pit != nodes.end());
        if (pit == nodes.end())
        {
//            assert(0);
            return false;
        }

        // Update strides of the same branch.
        // Iterate backwards from parent to the root and increment strides
        // ranging to the insertion
        //
        auto prit = pit;
        while (prit >= nodes.begin())
        {
            if (prit->m_branch_stride >/*=*/ (unsigned)std::distance(prit, pit/*+1*/)) // !!!
                prit->m_branch_stride++;
            if (!prit->m_parent_ofs) // discontinue past root (preceeding trees not affected)
                break;
            prit--;
        }
        
#if 0
        // Insert new node after its parent
        pit->m_nbr_children++;
        nodes.insert(pit+1, node); // wait with insertion if succeeding nodes are iterated also to update parent ofs'

#else
        // Update parent indices
        // Iterate forward up until the next root and increment parent ofs'
        // ranging to the insertion
        //
        auto pfit = pit+1;
        while (pfit < nodes.end())
        {
            if (!pfit->m_parent_ofs) // discontinue at succeeding root
                break;
            if (pfit->m_parent_ofs >= (unsigned)std::distance(pit, pfit))
                pfit->m_parent_ofs++;
            pfit++;
        }
        
        // Insert new node after its parent
        node.m_parent_ofs = 1;
        pit->m_nbr_children++;
        nodes.insert(pit+1, node);
//        data.insert(data.begin() + (pit+1-nodes.begin()), 123);
#endif
   
        return true;
    }
    
//    void erase(unsigned index)
//    {
//        // IF HAS PARENT
//        // Update strides of the same branch.
//        // Iterate backwards from parent to the root and increment strides
//        // ranging to the insertion
//        //
//        auto prit = pit;
//        while (prit >= nodes.begin())
//        {
//            if (prit->m_branch_stride >/*=*/ (unsigned)std::distance(prit, pit/*+1*/)) // !!!
//                prit->m_branch_stride++;
//            if (!prit->m_parent_ofs) // discontinue past root (preceeding trees not affected)
//                break;
//            prit--;
//        }
//
//        // Update parent indices
//        // Iterate forward up until the next root and increment parent ofs'
//        // ranging to the insertion
//        //
//        auto pfit = pit+1;
//        while (pfit < nodes.end())
//        {
//            if (!pfit->m_parent_ofs) // discontinue at succeeding root
//                break;
//            if (pfit->m_parent_ofs >= (unsigned)std::distance(pit, pfit))
//                pfit->m_parent_ofs++;
//            pfit++;
//        }
//
//        // Insert new node after its parent
//        node.m_parent_ofs = 1;
//        pit->m_nbr_children++;
//        nodes.insert(pit+1, node);
//        //        data.insert(data.begin() + (pit+1-nodes.begin()), 123);
//
//    }
    
    void reduce()
    {
        // if node[i+1] has 1 child, and node[i+1] is not tied to BONE or MESH
        //      MERGE node[i+1] (child) into into node[i]
        //      REMOVE node[i]
        //
        // MERGE = merge node transformations and keyframes(*) of all animations
        //
        // (*) if nbr keys is "1 or equal" between node[i] and node[i+1]
        
        for (int i = 0; i < nodes.size();)
        {
            // Abort if node has BONE or MESH
            if (nodes[i].bone_index > -1 || nodes[i].nbr_meshes > 0) { i++; continue; }
            // else if has 1 children and child is not
            
            if (nodes[i].m_nbr_children != 1) { i++; continue; }
            
            
            // Abort if child has BONE or MESH (first child is at i+1)
            //                if (nodes[i+1].bone_index > -1 || nodes[i+1].nbr_meshes > 0)
            //                {
            //                    i++;
            //                    continue;
            //                }
            
            // Now delete this node:
            
            auto node = nodes.begin()+i;
            
            // IF HAS PARENT
            // Update strides of the same branch.
            // Iterate backwards from parent to the root and increment strides
            // ranging to the insertion
            //
            auto prit = node; //-1;
            while (prit >= nodes.begin())
            {
                if (prit->m_branch_stride > /*>*/ (unsigned)std::distance(prit, node/*+1*/)) // !!!
                    prit->m_branch_stride--; //++;
                if (!prit->m_parent_ofs) // discontinue past root (preceeding trees not affected)
                    break;
                prit--;
            }
            
            // Update parent indices
            // Iterate forward up until the next root and increment parent ofs'
            // ranging to the insertion
            //
            auto pfit = node+1;
            while (pfit < nodes.end())
            {
                if (!pfit->m_parent_ofs) // discontinue at succeeding root
                    break;
                if (pfit->m_parent_ofs >=/*=*/ (unsigned)std::distance(node, pfit))
                    pfit->m_parent_ofs--; //++;
                pfit++;
            }
            
            nodes[i+1].local_tfm = nodes[i].local_tfm * nodes[i+1].local_tfm;
            // TODO
            // Mirror this merger in the keyframes for all animations:
            //  animation_t::std::vector<node_animation_t>
            
            //nodes[i].m_branch_stride = nodes[i+1].m_branch_stride;
            //nodes[i].m_nbr_children = nodes[i+1].m_nbr_children;
            nodes[i+1].m_parent_ofs = nodes[i].m_parent_ofs;
            std::cout << "REMOVED " << nodes[i].name << std::endl;
            nodes.erase(nodes.begin()+i);
            
        }
    }
    
    void raw_print_()
    {
        std::cout << "Tree dump" << std::endl;
        for (auto& e: nodes)
            std::cout
            << "\t"
            << (e.bone_index > -1 ? "BONE " + std::to_string(e.bone_index) + " ":"")
            << (e.nbr_meshes > 0 ? "MESHES " + std::to_string(e.nbr_meshes) + " ":"")
            << e.name << ","
            << e.m_nbr_children << ","
            << e.m_branch_stride << ","
            << e.m_parent_ofs << ")\n";
    }
    
    void raw_print()
    {
        for (auto& e: nodes)
            std::cout << "(" << e.name << ","
            << e.m_nbr_children << ","
            << e.m_branch_stride << ", "
            << e.m_parent_ofs << ") ";
        std::cout << std::endl;
    }
    
    // Dump tree(s) to stream
    void debug_print(logstreamer_t&& outstream)
    {
        int i = 0;
        while (i < nodes.size())
        {
            debug_print(i, "", outstream);
            i += nodes[i].m_branch_stride;
        }
    }
    
private:
    
    // Dump branch (e.g. from a root) to stream
    // Recursive, depth-first traversal
    void debug_print(unsigned i, const std::string& indent, logstreamer_t& outstream)
    {
        auto& node = nodes[i];
        outstream << indent;
        outstream << " [node " << i << "]";
        if ( node.bone_index > -1 ) outstream << "[bone " << node.bone_index <<"]";
        if ( node.nbr_meshes ) outstream << "[" << node.nbr_meshes << " meshes]";
        outstream << " " << node.name
        << " (children " << node.m_nbr_children
        << ", stride " << node.m_branch_stride
        << ", parent ofs " << node.m_parent_ofs << ")";
#if 1
        if (node.attribs.size())
            outstream << ", xiattr = " << node.attribs;
#endif
#if 0
        // + print certain attribs
        if (node.JointName_attrib.size())
            outstream << ", JointName = " << node.JointName_attrib;
        if (node.EntityType_attrib.size())
            outstream << ", EntityType = " << node.EntityType_attrib;
        if (node.EntityID_attrib.size())
            outstream << ", EntityID = " << node.EntityID_attrib;
        if (node.MeshType_attrib.size())
            outstream << ", MeshType = " << node.MeshType_attrib;
#endif
        outstream << std::endl;
#if 0
        const auto& m = node.local_tfm;
        for (int i=0; i<4; i++) {
            outstream << m.mat[0][i] << ", " << m.mat[1][i] << ", ";
            outstream << m.mat[2][i] << ", " << m.mat[3][i] << ", " << std::endl;
        }
#endif

        int ci = i + 1;
        for (int j = 0; j < node.m_nbr_children; j++)
        {
            debug_print(ci, indent+"\t", outstream);
            ci += nodes[ci].m_branch_stride;
        }
    }
};

#endif /* seqtree_h */
