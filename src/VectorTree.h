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

// Remove when SkeletonNode is moved out
#include <glm/glm.hpp>

#include "config.h"
#include "vec.h"
#include "mat.h"
#include "logstreamer.h"

using linalg::m4f;
using linalg::m4f_1;
using namespace logstreamer;

struct TreeNode
{
    unsigned m_nbr_children = 0;  ///< Nbr of children
    unsigned m_branch_stride = 1; ///< Branch size including this node
    unsigned m_parent_ofs = 0;    ///< Distance to parent, relative parent.
};

struct SkeletonNode : public TreeNode
{
    glm::mat4 local_tfm;        ///< Transform relative parent
    glm::mat4 global_tfm{1.0f}; ///< Updated during tree traversal
    int bone_index = EENG_NULL_INDEX;
    int nbr_meshes = 0;
    std::string name = "";

    SkeletonNode() = default;
    SkeletonNode(const std::string &name, const glm::mat4 &local_tfm)
        : name(name),
          local_tfm(local_tfm) {}
};

/// @brief Sequential tree representation optimized for depth-first traversal
/// @tparam NodeType Node type, should inherit from TreeNode
/** Nodes are organized in pre-order, which means that the first child of a
 * node is located directly after the node. Each node has information about
 * number children, stride of its branch, and offset from its parent.
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
        auto it = std::find_if(nodes.begin(),
                               nodes.end(),
                               [&node_name](const NodeType &node)
                               { return !node.name.compare(node_name); });
        if (it == nodes.end())
            return EENG_NULL_INDEX;
        return std::distance(nodes.begin(), it);
    }

    /// @brief Insert a node
    /// @param node Node to insert
    /// @param parent Name of parent node. If empty, node is inserted as a root
    /// @return True if insertion was successfull, false otherwise
    bool insert(NodeType node, const std::string &parent_name)
    {
        // No parent given - insert as root
        if (!parent_name.size())
        {
            nodes.insert(nodes.begin(), node);
            return true;
        }

        // Locate parent
        auto parent_index = find_node_index(parent_name);
        if (parent_index == EENG_NULL_INDEX)
            return false;
        auto pit = nodes.begin() + parent_index;

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

        return true;
    }

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
        if (node.bone_index != EENG_NULL_INDEX)
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
