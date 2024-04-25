//
//  vectordag.h
//  tau3d
//
//  Created by Carl Johan Gribel on 2016-02-15.
//
//

#ifndef VECTORTREE_H
#define VECTORTREE_H

#include <iostream>
#include <vector>
#include "vec.h"
#include "mat.h"
#include "logstreamer.h"

using linalg::m4f;
using linalg::m4f_1;
using namespace logstreamer;

struct TreeNode
{
    unsigned m_nbr_children = 0;  // Nbr of children
    unsigned m_branch_stride = 1; // Branch size including this node
    unsigned m_parent_ofs = 0;    // Distance to parent, relative parent.
};

struct SkeletonNode : public TreeNode
{
    m4f local_tfm;          // Transform relative parent
    m4f global_tfm = m4f_1; // Updated during tree traversal
    int bone_index = -1;
    int nbr_meshes = 0;
    std::string name = "";

    SkeletonNode() = default;
    SkeletonNode(const std::string &name, const m4f &local_tfm)
        : name(name),
          local_tfm(local_tfm) {}
};

/**
Sequential tree representation optimized for depth-first traversal.
Nodes are organized in pre-order,
which means that the first child of a node is located directly after the node.
Each node has information about number children, stride of its branch, and offset from its parent.
The tree can be traversed both up and down.
*/
template <class NodeType>
class VectorTree
{
public:
    std::vector<NodeType> nodes;

    VectorTree() = default;

    /// @brief Find index of a node by name
    /// @param node_name Node name to search for
    /// @return Index Node index
    size_t find_node_index(const std::string &node_name)
    {
        auto it = std::find_if(nodes.begin(), nodes.end(),
                               [&node_name](const NodeType &node)
                               { return !node.name.compare(node_name); });
        if (it == nodes.end())
            return -1;
        return std::distance(nodes.begin(), it);
    }

    /// @brief 
    /// @param node_name 
    /// @return 
    // NodeType *find_node(const std::string &node_name)
    // {
    //     const auto index = find_node_index(node_name);
    //     if (index == -1)
    //         return nullptr;
    //     return &nodes[index];

    //     // auto it = std::find_if(nodes.begin(), nodes.end(),
    //     //                        [&node_name](const NodeType &node)
    //     //                        { return !node.name.compare(node_name); });
    //     // if (it == nodes.end())
    //     //     return nullptr;
    //     // return &(*it);
    // }

    /// @brief Insert a node
    /// @param node Node to insert
    /// @param parent Name of parent node. If empty, node is inserted as a root
    /// @return True if insertion was successfull, false otherwise
    bool insert(NodeType node, const std::string &parent)
    {
        // No parent given - insert as root
        if (!parent.size())
        {
            nodes.insert(nodes.begin(), node);
            return true;
        }

        // Locate parent
        const std::function<bool(const NodeType &)> findparent_lambda =
            [&](const NodeType &node)
        {
            return !node.name.compare(parent);
        };
        auto pit = std::find_if(nodes.begin(), nodes.end(), findparent_lambda);

        // Parent not found
        if (pit == nodes.end())
            return false;

        // Update branch strides within the same branch
        // Iterate backwards from parent to the root and increment strides
        // ranging to the insertion
        //
        auto prit = pit;
        while (prit >= nodes.begin())
        {
            if (prit->m_branch_stride > /*=*/(unsigned)std::distance(prit, pit /*+1*/)) // !!!
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
        auto pfit = pit + 1;
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
        nodes.insert(pit + 1, node);
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
            if (nodes[i].bone_index > -1 || nodes[i].nbr_meshes > 0)
            {
                i++;
                continue;
            }
            // else if has 1 children and child is not

            if (nodes[i].m_nbr_children != 1)
            {
                i++;
                continue;
            }

            // Abort if child has BONE or MESH (first child is at i+1)
            //                if (nodes[i+1].bone_index > -1 || nodes[i+1].nbr_meshes > 0)
            //                {
            //                    i++;
            //                    continue;
            //                }

            // Now delete this node:

            auto node = nodes.begin() + i;

            // IF HAS PARENT
            // Update strides of the same branch.
            // Iterate backwards from parent to the root and increment strides
            // ranging to the insertion
            //
            auto prit = node; //-1;
            while (prit >= nodes.begin())
            {
                if (prit->m_branch_stride > /*>*/ (unsigned)std::distance(prit, node /*+1*/)) // !!!
                    prit->m_branch_stride--;                                                  //++;
                if (!prit->m_parent_ofs)                                                      // discontinue past root (preceeding trees not affected)
                    break;
                prit--;
            }

            // Update parent indices
            // Iterate forward up until the next root and increment parent ofs'
            // ranging to the insertion
            //
            auto pfit = node + 1;
            while (pfit < nodes.end())
            {
                if (!pfit->m_parent_ofs) // discontinue at succeeding root
                    break;
                if (pfit->m_parent_ofs >= /*=*/(unsigned)std::distance(node, pfit))
                    pfit->m_parent_ofs--; //++;
                pfit++;
            }

            nodes[i + 1].local_tfm = nodes[i].local_tfm * nodes[i + 1].local_tfm;
            // TODO
            // Mirror this merger in the keyframes for all animations:
            //  animation_t::std::vector<node_animation_t>

            // nodes[i].m_branch_stride = nodes[i+1].m_branch_stride;
            // nodes[i].m_nbr_children = nodes[i+1].m_nbr_children;
            nodes[i + 1].m_parent_ofs = nodes[i].m_parent_ofs;
            std::cout << "REMOVED " << nodes[i].name << std::endl;
            nodes.erase(nodes.begin() + i);
        }
    }

    // void raw_print_()
    // {
    //     std::cout << "Tree dump" << std::endl;
    //     for (auto &e : nodes)
    //         std::cout
    //             << "\t"
    //             << (e.bone_index > -1 ? "bones " + std::to_string(e.bone_index) + " " : "")
    //             << (e.nbr_meshes > 0 ? "meshes " + std::to_string(e.nbr_meshes) + " " : "")
    //             << e.name << ","
    //             << e.m_nbr_children << ","
    //             << e.m_branch_stride << ","
    //             << e.m_parent_ofs << ")\n";
    // }

    // void raw_print()
    // {
    //     for (auto &e : nodes)
    //         std::cout << "(" << e.name << ","
    //                   << e.m_nbr_children << ","
    //                   << e.m_branch_stride << ", "
    //                   << e.m_parent_ofs << ") ";
    //     std::cout << std::endl;
    // }

    /// Dump all roots to a stream using recursive traversal
    void debug_print(logstreamer_t &&outstream)
    {
        int i = 0;
        while (i < nodes.size())
        {
            debug_print(i, "", outstream);
            i += nodes[i].m_branch_stride;
        }
    }

private:
    /// Dump branch to a stream using recursive traversal
    void debug_print(unsigned i, const std::string &indent, logstreamer_t &outstream)
    {
        auto &node = nodes[i];
        outstream << indent;
        outstream << " [node " << i << "]";
        if (node.bone_index > -1)
            outstream << "[bone " << node.bone_index << "]";
        if (node.nbr_meshes)
            outstream << "[" << node.nbr_meshes << " meshes]";
        outstream << " " << node.name
                  << " (children " << node.m_nbr_children
                  << ", stride " << node.m_branch_stride
                  << ", parent ofs " << node.m_parent_ofs << ")";
        outstream << std::endl;
        int ci = i + 1;
        for (int j = 0; j < node.m_nbr_children; j++)
        {
            debug_print(ci, indent + "\t", outstream);
            ci += nodes[ci].m_branch_stride;
        }
    }
};

#endif /* seqtree_h */
