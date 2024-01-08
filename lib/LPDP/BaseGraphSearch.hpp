/******************************************************************************
 * BaseGraphSearch.hpp
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
 
#ifndef LP_BASEGRAPHSEARCH_HPP
#define LP_BASEGRAPHSEARCH_HPP

#include "Block.hpp"

class BaseGraphSearch;

class SearchBranch {
public:
    SearchBranch(BaseBlock &block, PathMap* result_map);
    void set_branch(NodeID cur_start_node);

    //set parallel = true if you execute multiple SearchBranches at the same time
    STEP_COUNT run(PathMap* result_map, bool parallel);
    STEP_COUNT split_work(std::vector<SearchBranch>* search_branches, unsigned steps);
    STEP_COUNT init_map(PathMap* result_map) {
        //only call after branch was freshly generated
        step_counter = 0;
        this->result_map = result_map;
        update_solutions_help = &SearchBranch::ser_update_solutions;
        update_solutions();
        return step_counter;
    }

private:

    void search(NodeID cur_node);
    void search(NodeID cur_node, std::vector<SearchBranch>* search_branches, unsigned steps);
    void update_solutions();

    //if solved serially we can the solutions of a base block more efficiently
    //(this is not the case for higher level blocks)
    void ser_update_solutions(Index pos);
    void ser_update_solutions_no_test(Index pos);

    //parallel update prevents race conditions, but is less efficient
    void par_update_solutions(Index pos);

    //update_solutions() calls this pointer. is changed to &par_update_solutions_help if branch is solved in parallel
    void (SearchBranch::*update_solutions_help)(Index) = &SearchBranch::ser_update_solutions;

    BaseBlock& block;
    PathMap* result_map;
    std::vector<bool> marked;
    //boundary node is unmarked and has 2 boundary edges to other blocks => we have to calculate solutions that don't include the node
    //1 boundary edge is enough if the boundry node is the start or target vertex.
    std::vector<NodeID> possible_excluded_nodes;
    NodeID cur_start_node; //starting (boundary) node of the current path segment
    BNodePairs cur_matching;
    Path cur_path;

    //run() starts the search with search(branch_start_node);_
    NodeID branch_start_node;

    STEP_COUNT step_counter;
};

class BaseGraphSearch {
public:

    BaseGraphSearch(BaseBlock &block);

    STEP_COUNT run(unsigned number_of_threads, unsigned split_steps, unsigned threshold);

private:
    friend class SearchBranch;

    BaseBlock& block;

    STEP_COUNT step_counter;
};
#endif //LP_BASEGRAPHSEARCH_HPP
