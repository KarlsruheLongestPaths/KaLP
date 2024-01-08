/******************************************************************************
 * BlockGraphSearch.hpp
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

#ifndef LP_BLOCKGRAPHSEARCH_HPP
#define LP_BLOCKGRAPHSEARCH_HPP

#include "Block.hpp"

class BlockGraphSearch;

class BGSearchBranch {
public:
    BGSearchBranch(BlockGraphSearch &cur_search, Block &block, PathMap* result_map);
    void set_branch(NodeID cur_start_node);

    STEP_COUNT run(PathMap* result_map);
    STEP_COUNT split_work(std::vector<BGSearchBranch>* search_branches, unsigned steps);
    STEP_COUNT init_map(PathMap* result_map) {
        //only call after branch was freshly generated
        step_counter = 0;
        this->result_map = result_map;
        update_solutions();
        return step_counter;
    }
private:
    void search(BlockID subblock_id, NodeID node, bool switch_subblock);
    void search(BlockID subblock_id, NodeID node, bool switch_subblock, std::vector<BGSearchBranch>* search_branches, unsigned steps);
    void update_solutions();
    void update_solutions_help(Index pos);

    inline std::pair<BlockID, NodeID> convert(NodeID boundary_node) {
        ensure(boundary_node < block.number_of_boundary_nodes, "not a boundary node");
        return std::make_pair(convert_to_subblock[boundary_node], convert_to_subnode[boundary_node]);
    };

    Block& block;
    PathMap* result_map;
    std::vector<std::vector<bool>> marked;
    //boundary node is unmarked and has 2 boundary edges to other blocks => we have to calculate solutions that don't include the node
    //1 boundary edge is enough if the boundry node is the start or target vertex.
    std::vector<NodeID> possible_excluded_nodes;
    NodeID cur_start_node; //starting (boundary) node of the current path segment
    BNodePairs cur_matching;
    Path cur_path;

    const std::vector<NodeID> &convert_to_subnode;
    const std::vector<BlockID> &convert_to_subblock;

    //run() starts the search with search(branch_start_block, branch_start_node, branch_switch_subblock);_
    BlockID branch_start_block;
    NodeID branch_start_node;
    bool branch_switch_subblock;//run() starts the search from branch_start_node

    STEP_COUNT step_counter;
};

class BlockGraphSearch {
public:

    BlockGraphSearch(Block &block);

    STEP_COUNT run(unsigned number_of_threads, unsigned split_steps, unsigned threshold);

private:
    friend class BGSearchBranch;

    Block& block;
    std::vector<NodeID> convert_to_subnode;
    std::vector<BlockID> convert_to_subblock;

    STEP_COUNT step_counter;
};

#endif //LP_BLOCKGRAPHSEARCH_HPP
