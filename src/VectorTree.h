// Created by Carl Johan Gribel.
// Licensed under the MIT License. See LICENSE file for details.

#ifndef VECTORTREE_H
#define VECTORTREE_H

#include <iostream>
#include <vector>
#include "config.h"

namespace eeng
{
    struct TreeNode
    {
        unsigned m_nbr_children = 0;  ///< Nbr of children
        unsigned m_branch_stride = 1; ///< Branch size including this node
        unsigned m_parent_ofs = 0;    ///< Distance to parent, relative parent.
    };

    /// @brief Contiguous tree representation optimized for depth-first traversal
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
                if (prit->m_branch_stride > (unsigned)std::distance(prit, pit))
                    prit->m_branch_stride++;
                if (!prit->m_parent_ofs) // Discontinue past root (preceeding trees not affected)
                    break;
                prit--;
            }

            // Update parent indices
            // Iterate forward up until the next root, 
            // and increment parent offsets ranging to the insertion
            //
            auto pfit = pit + 1;
            while (pfit < nodes.end())
            {
                if (!pfit->m_parent_ofs) // Discontinue at succeeding root
                    break;
                if (pfit->m_parent_ofs >= (unsigned)std::distance(pit, pfit))
                    pfit->m_parent_ofs++;
                pfit++;
            }

            // Insert new node after its parent
            node.m_parent_ofs = 1;
            pit->m_nbr_children++;
            nodes.insert(pit + 1, node);

            return true;
        }
    };
}
#endif /* VectorTree */
