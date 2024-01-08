/******************************************************************************
 * QueryManager.hpp
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

#ifndef LP_QUERYMANAGER_HPP
#define LP_QUERYMANAGER_HPP

#include "Block.hpp"
#include "undirected_graph.h"
#include <sys/time.h>

struct Result {
    std::vector<NodeID> path;
    Weight weight;
};

class QueryManager {
private:
    unsigned graph_size;
    NodeID start_node;
    NodeID end_node;

    std::vector<std::vector<Block*>> blocks;

    bool solvable;

    void dfs(UndirectedGraph *graph, const int node, std::vector<bool> &marked) const;
    void unpack(Block* block, BNodePairs& key, std::vector<std::vector<NodeID>> &path_segments);
public:
    QueryManager(UndirectedGraph *graph, std::vector<std::vector<int>> &partitions, const int start, const int goal);
    Result run(unsigned number_of_threads, unsigned split_steps, unsigned threshold);
};



#endif //LP_QUERYMANAGER_HPP
