/******************************************************************************
 * Block.hpp
 *
 * Source of KaLP -- Karlsruhe Longest Paths 
 ******************************************************************************
 * Copyright (C) 2018 Kai Fieger
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/
 
#ifndef LP_BLOCK_HPP
#define LP_BLOCK_HPP

#include <vector>
#include <unordered_map>
#include "constants.hpp"
#include "PathMap.hpp"

struct BoundaryEdge {
    NodeID target_node;
    BlockID target_block;
    Weight weight;
};
class BoundaryEdgeIterator;


class BoundaryEdgeRange {
public:
    BoundaryEdgeRange(const std::vector<NodeID> &target_node, const std::vector<BlockID> &target_block, const std::vector<Weight> &weight, unsigned begin_pos, unsigned end_pos);
    BoundaryEdgeIterator begin() const;
    BoundaryEdgeIterator end() const;
    //TODO not used?
    unsigned size() const;
private:
    const std::vector<NodeID> *target_node;
    const std::vector<BlockID> *target_block;
    const std::vector<Weight> *weight;
    unsigned begin_pos;
    unsigned end_pos;
};

class BoundaryEdgeIterator {
public:
    BoundaryEdgeIterator(const std::vector<NodeID> &target_node, const std::vector<BlockID> &target_block, const std::vector<Weight> &weight, unsigned pos);
    BoundaryEdgeIterator& operator++();
    const BoundaryEdge& operator*();
    //TODO other -> const?
    bool operator==(BoundaryEdgeIterator& other) const;
    bool operator!=(BoundaryEdgeIterator& other) const;
private:
    const std::vector<NodeID> *target_node;
    const std::vector<BlockID> *target_block;
    const std::vector<Weight> *weight;
    unsigned pos;
    BoundaryEdge boundary_edge;
};



class IndexRange;
class IndexIterator;

class IndexRange {
public:
    IndexRange(unsigned begin_pos, unsigned end_pos);
    IndexIterator begin() const;
    IndexIterator end() const;
    //TODO not used?
    unsigned size() const;
private:
    unsigned begin_pos;
    unsigned end_pos;
};

class IndexIterator {
public:
    IndexIterator(unsigned pos);
    IndexIterator& operator++();
    const unsigned operator*();
    //TODO other -> const?
    bool operator==(IndexIterator& other) const;
    bool operator!=(IndexIterator& other) const;
private:
    unsigned pos;
};

class Block {
public:
    Block* parent_block = nullptr;
    std::vector<Block*> subblocks;

    unsigned number_of_boundary_nodes = 0;
    std::vector<NodeID> boundary_nodes_in_parent;
    std::vector<NodeID> boundary_node_to_input_ids;
    std::vector<NodeID> connected_by_multiple_boundary_edges;

    PathMap pathmap;

    //boundary_edges // edges outside the current block (or between subblocks.?)
    std::vector<unsigned> first_out;
    std::vector<NodeID> target_node;
    std::vector<BlockID> target_block;
    std::vector<Weight> weight;

    IndexRange boundary_nodes();
    BoundaryEdgeRange boundary_edges(NodeID node);

    bool solved = false;
    virtual STEP_COUNT solve_block();
    virtual STEP_COUNT solve_block(unsigned number_of_threads, unsigned split_steps, unsigned threshold);
    //void block_graph_search();
};

class BaseBlock : public Block {
public:
    //subgraph of the underlying graph that makes up the block. the boundary nodes of the block are 0 to number_of_boundary_nodes-1
    std::vector<unsigned> graph_first_out;
    std::vector<NodeID> graph_target_node;
    std::vector<Weight> graph_weight;

    std::vector<NodeID> input_ids;

    STEP_COUNT  solve_block();
    STEP_COUNT  solve_block(unsigned number_of_threads, unsigned split_steps, unsigned threshold);
    //void base_graph_search();
};

#endif //LP_BLOCK_HPP
