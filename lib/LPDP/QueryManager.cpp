/******************************************************************************
 * QueryManager.cpp
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

#include "QueryManager.hpp"

void QueryManager::dfs(UndirectedGraph *graph, const int node, std::vector<bool> &marked) const {
    marked[node] = true;
    for (EdgeLP& edge : graph->nodes[node]) {
        if (!marked[edge.nextVertex]) {
            dfs(graph, edge.nextVertex, marked);
        }
    }
}

QueryManager::QueryManager(UndirectedGraph *graph, std::vector<std::vector<int>> &partitions, const int start, const int goal) : graph_size(graph->size()), blocks(partitions.size() - 1) {
    start_node = start;
    end_node = goal;
    for (BlockID block_id = 0; block_id < partitions[1].size(); block_id++) {
        blocks[0].push_back(new BaseBlock());
    }
    for (Level l = 1; l < blocks.size(); l++) {
        for (BlockID block_id = 0; block_id < partitions[l+1].size(); block_id++) {
            blocks[l].push_back(new Block());
        }
    }

    //connect block with parent&subblocks
    for (Level l = 1; l < blocks.size(); l++) {
        for (BlockID subblock_id = 0; subblock_id < partitions[l].size(); subblock_id++) {
            blocks[l][partitions[l][subblock_id]]->subblocks.push_back(blocks[l-1][subblock_id]);
            blocks[l-1][subblock_id]->parent_block = blocks[l][partitions[l][subblock_id]];
        }
    }
    blocks.back()[0]->parent_block = nullptr;

    //block_per_level[level][node] = block ID in which the node "node" is on level "level"
    std::vector<std::vector<BlockID>> block_per_level(blocks.size());
    for (Level l = 0; l < blocks.size(); l++) {
        block_per_level[l] = std::vector<BlockID>(graph->nodes.size());
    }
    for (NodeID node = 0; node < graph->nodes.size(); node++) {
        BlockID block_id = node;
        for (Level l = 0; l < blocks.size(); l++) {
            block_id = partitions[l][block_id];
            block_per_level[l][node] = block_id;
        }
    }

    std::vector<bool> reachable(graph->size(), false);
    dfs(graph, start, reachable);

    this->solvable = reachable[goal];

    std::vector<std::vector<NodeID>> boundary_nodes(blocks.size());
    for (Level l = 0; l < boundary_nodes.size(); l++) {
        boundary_nodes[l] = std::vector<NodeID>(graph->nodes.size(), invalid_node);
    }
    for (NodeID node = 0; node < graph->nodes.size(); node++) {
        Level max_level = 0; //node is boundary node on max_level - 1 but not on max_level
        if (!reachable[node]) {
            max_level = 0; //=> no boundary nodes that are unreachable from start (or target) => no calculations for unreachable blocks/areas of the graph
        } else if (node == start || node == goal) {
            max_level = blocks.size();
        } else {
            for (; max_level < blocks.size(); max_level++) {
                bool is_boundary_node = false;
                BlockID src_block = block_per_level[max_level][node];
                for (EdgeLP &edge : graph->nodes[node]) {
                    BlockID dst_block = block_per_level[max_level][edge.nextVertex];
                    if (src_block != dst_block) {
                        is_boundary_node = true;
                        break;
                    }
                }
                if (!is_boundary_node) {
                    break;
                }
            }
        }
        for (Level l = 0; l < max_level; l++) {
            boundary_nodes[l][node] = blocks[l][block_per_level[l][node]]->number_of_boundary_nodes++;
            blocks[l][block_per_level[l][node]]->boundary_node_to_input_ids.push_back(node);
        }
    }

    //
    std::vector<std::vector<BlockID>> local_block_id(blocks.size());
    for (Level l = 0; l < blocks.size(); l++) {
        local_block_id[l] = std::vector<BlockID>(blocks[l].size());
    }
    for (Level l = 0; l + 1 < blocks.size(); l++) {
        for (BlockID block_id = 0; block_id < blocks[l].size(); block_id++) {
            Block* parent = blocks[l][block_id]->parent_block;
            for (BlockID local_id = 0; local_id < parent->subblocks.size(); local_id++) {
                if (parent->subblocks[local_id] == blocks[l][block_id]) {
                    local_block_id[l][block_id] = local_id;
                    break;
                }
            }
        }
    }

    std::vector<unsigned> number_of_nodes(blocks[0].size());
    std::vector<NodeID> transform_nodes = boundary_nodes[0];
    for (BlockID block_id = 0; block_id < blocks[0].size(); block_id++) {
        number_of_nodes[block_id] = blocks[0][block_id]->number_of_boundary_nodes;
    }
    for (NodeID node = 0; node < transform_nodes.size(); node++) {
        if (transform_nodes[node] == invalid_node && reachable[node]) {
            transform_nodes[node] = number_of_nodes[block_per_level[0][node]]++;
        }
    }

    //add edges to the blocks (level 0 graphs and boundary edges between the higher level blocks)
    for (NodeID node = 0; node < graph->nodes.size(); node++) {
        if (reachable[node]) {
            for (Level l = 0; l < blocks.size() && boundary_nodes[l][node] != invalid_node; l++) {
                Block *block = blocks[l][block_per_level[l][node]];
                block->first_out.push_back(block->target_node.size());
                if (l == 0) {
                    BaseBlock *level0block = static_cast<BaseBlock *>(blocks[l][block_per_level[l][node]]);
                    level0block->graph_first_out.push_back(level0block->graph_target_node.size());
                    level0block->input_ids.push_back(node);
                }
            }

            for (EdgeLP &edge : graph->nodes[node]) {
                Level max_level = 0;
                for (; max_level < blocks.size(); max_level++) {
                    BlockID src_block = block_per_level[max_level][node];
                    BlockID dst_block = block_per_level[max_level][edge.nextVertex];
                    if (src_block == dst_block) {
                        break;
                    }
                }
                if (max_level) {
                    max_level--;

                    Block *block = blocks[max_level][block_per_level[max_level][node]];

                    block->target_node.push_back(boundary_nodes[max_level][edge.nextVertex]);
                    block->target_block.push_back(
                            local_block_id[max_level][block_per_level[max_level][edge.nextVertex]]);
                    block->weight.push_back(edge.weight);
                } else {
                    BaseBlock *block = static_cast<BaseBlock *>(blocks[0][block_per_level[0][node]]);

                    if (transform_nodes[node] < block->number_of_boundary_nodes) {
                        block->graph_target_node.push_back(transform_nodes[edge.nextVertex]);
                        block->graph_weight.push_back(edge.weight);
                    }
                }
            }
        }
    }

    for (NodeID node = 0; node < graph->nodes.size(); node++) {
        if (reachable[node]) {
            if (boundary_nodes[0][node] == invalid_node) {
                BaseBlock *level0block = static_cast<BaseBlock *>(blocks[0][block_per_level[0][node]]);
                level0block->graph_first_out.push_back(level0block->graph_target_node.size());
                level0block->input_ids.push_back(node);
            }
            BlockID src_block = block_per_level[0][node];
            for (EdgeLP &edge : graph->nodes[node]) {
                BlockID dst_block = block_per_level[0][edge.nextVertex];
                if (src_block == dst_block) {
                    BaseBlock *block = static_cast<BaseBlock *>(blocks[0][block_per_level[0][node]]);
                    if (transform_nodes[node] >= block->number_of_boundary_nodes) {
                        block->graph_target_node.push_back(transform_nodes[edge.nextVertex]);
                        block->graph_weight.push_back(edge.weight);
                    }
                }
            }
        }
    }

    for (Block* block : blocks[0]) {
        BaseBlock* level0block = static_cast<BaseBlock*>(block);
        level0block->graph_first_out.push_back(level0block->graph_target_node.size());
    }
    for (Level l = 0; l < blocks.size(); l++) {
        for (Block* block : blocks[l]) {
            block->first_out.push_back(block->target_node.size());
        }
    }

    //store transformation to parent boundary nodes for each block
    for (NodeID node = 0; node < graph->nodes.size(); node++) {
        for (Level l = 0; l+1 < blocks.size(); l++) {
            if (boundary_nodes[l][node] != invalid_node) {
                blocks[l][block_per_level[l][node]]->boundary_nodes_in_parent.push_back(boundary_nodes[l+1][node]);
            }
        }
    }

    for (Level l = 0; l < blocks.size(); l++) {
        for (auto& block : blocks[l]) {
            unsigned counter = 0;

            block->connected_by_multiple_boundary_edges = std::vector<NodeID>(block->number_of_boundary_nodes, invalid_node);

            for (NodeID node : block->boundary_nodes()) {
                Block* cur_block = block;
                NodeID cur_node = node;
                unsigned boundary_edges_across_levels = cur_block->boundary_edges(cur_node).size();
                while (cur_block->parent_block != nullptr && cur_block->boundary_nodes_in_parent[cur_node] != invalid_node && boundary_edges_across_levels < 2) {
                    cur_node = cur_block->boundary_nodes_in_parent[cur_node];
                    cur_block = cur_block->parent_block;
                    boundary_edges_across_levels += cur_block->boundary_edges(cur_node).size();
                }
                //if in the highest level & still a boundary node => start or target node => boundary_edges_across_levels++ as the s/t node only need 1 boundary node in order to be excluded
                if (cur_block->parent_block == nullptr) {
                    boundary_edges_across_levels++;
                }
                if (boundary_edges_across_levels >= 2) {
                    block->connected_by_multiple_boundary_edges[node] = counter++;
                }
            }
        }
    }

    //1 block exists in multiple levels => remove duplicates, only 1 pathmap needed
    Level l = blocks.size();
    while (l) {
        l--;
        for (BlockID block_id = 0; block_id < blocks[l].size(); block_id++) {
        //for (Block* block : blocks[l]) {
            Block* block = blocks[l][block_id];
            Block* cur_block = block;
            int duplicates = 0;
            while (cur_block->subblocks.size() == 1) {
                duplicates++;
                cur_block = cur_block->subblocks[0];
            }
            if (cur_block != block) {
                Block* parent = block->parent_block;
                //parents subblock pointer redirected to the bottom duplicate
                if (parent != nullptr) {
                    for (BlockID subblock_id = 0; subblock_id < parent->subblocks.size(); subblock_id++) {
                        if (parent->subblocks[subblock_id] == block) {
                            parent->subblocks[subblock_id] = cur_block;
                        }
                    }
                }
                //merge bottom with top duplicate => now bottom duplicate only valid one
                cur_block->parent_block = block->parent_block;
                cur_block->boundary_nodes_in_parent = block->boundary_nodes_in_parent;
                cur_block->first_out = block->first_out;
                cur_block->target_node = block->target_node;
                cur_block->target_block = block->target_block;
                cur_block->weight = block->weight;

                //delete other duplicates (& replace them with the valid one)
                BlockID cur_block_id;
                for (BlockID b_id = 0; b_id < blocks[l - duplicates].size(); b_id++) {
                    if (cur_block == blocks[l - duplicates][b_id]) {
                        cur_block_id = b_id;
                        break;
                    }
                }
                for (Level cur_level = l + 1 - duplicates; cur_level < l + 1; cur_level++) {
                    cur_block_id = partitions[cur_level][cur_block_id];
                    delete blocks[cur_level][cur_block_id];
                    blocks[cur_level][cur_block_id] = cur_block;
                }
            }
        }
    }
}

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int getValue(){ //Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmSize:", 7) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}

#include <chrono>
//spit_steps only matter if number_of_threads > 1
Result QueryManager::run(unsigned number_of_threads, unsigned split_steps, unsigned threshold) {
    if (!solvable) {
        return {{}, 0};
    }
    STEP_COUNT step_counter = 0;
    std::vector<std::vector<unsigned>> threads_per_block(blocks.size());
    for (Level l = 0; l < blocks.size(); l++) {
        threads_per_block[l] = std::vector<unsigned>(blocks[l].size(), 1);
        if (l != 0) {
            for (BlockID block_id = 0; block_id < blocks[l].size(); block_id++) {
                unsigned number_sub_boundary_nodes = 0;
                for (auto subblock : blocks[l][block_id]->subblocks) {
                    number_sub_boundary_nodes += subblock->number_of_boundary_nodes;
                }
                //TODO better
                if (number_sub_boundary_nodes >= threshold) {
                    threads_per_block[l][block_id] = number_of_threads;
                }
            }
        } else {
            for (BlockID block_id = 0; block_id < blocks[l].size(); block_id++) {
                BaseBlock* level0block = static_cast<BaseBlock*>(blocks[l][block_id]);
                unsigned number_of_nodes = level0block->graph_first_out.size() - 1;
                //TODO number of nodes OR number of boundary nodes
                //if (level0block->number_of_boundary_nodes >= threshold) {
                if (number_of_nodes >= threshold) {
                    threads_per_block[l][block_id] = number_of_threads;
                }
            }
        }
    }

    std::vector<STEP_COUNT> thread_step_counter(number_of_threads, 0);
    for (Level l = 0; l < blocks.size(); l++) {
        // test block parallel or not
        //based on boundary nodes and boundary nodes of subblocks


        const auto query_start = std::chrono::high_resolution_clock::now();

        //solve easy blocks (multiple blocks at a time, but each block serially)
        omp_set_num_threads(number_of_threads);
        #pragma omp parallel for schedule(dynamic, 1)
        for (BlockID block_id = 0; block_id < blocks[l].size(); block_id++) {
            if (threads_per_block[l][block_id] == 1) {
                thread_step_counter[omp_get_thread_num()] += blocks[l][block_id]->solve_block();
                std::cout << "done" << std::endl;
            }
        }

        //solve difficult blocks (one block after another, each block in parallel)
        //TODO OR every block with max_threads / num_blocks
        for (BlockID block_id = 0; block_id < blocks[l].size(); block_id++) {
            if (threads_per_block[l][block_id] > 1) {
                step_counter += blocks[l][block_id]->solve_block(threads_per_block[l][block_id], split_steps, threshold);
            }
        }

        const auto query_end = std::chrono::high_resolution_clock::now();
        const double calcTime = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(query_end - query_start).count();
        std::cout << "Level " << l << " solved in " << calcTime << " sec" << std::endl;
        std::cout << std::endl;
    }
    for (STEP_COUNT counter : thread_step_counter) {
        step_counter += counter;
    }

    BNodePairs longest_path(2);
    longest_path.insert(0,1);
    /* should not happen, as this is checked with if(!solvable) at the beginning of the method
    if (!blocks.back().back()->pathmap.count(longest_path)) {
        return {{}, 0};
    }
    */
    std::vector<std::vector<NodeID>> path_segments;
    Result result{{}, blocks.back().back()->pathmap.get_weight(longest_path)};

    unpack(blocks.back().back(), longest_path, path_segments);

    result.path.push_back(start_node);
    std::vector<std::vector<std::vector<NodeID>*>> path_segments_per_node(graph_size);
    for (auto& segment : path_segments) {
        path_segments_per_node[segment.front()].push_back(&segment);
        path_segments_per_node[segment.back()].push_back(&segment);
    }

    std::vector<NodeID> nothing = {};
    std::vector<NodeID> *last_segment =  &nothing;
    while (result.path.back() != end_node) {
        std::vector<NodeID> *cur_segment = path_segments_per_node[result.path.back()].front();
        if (cur_segment == last_segment) {
            cur_segment = path_segments_per_node[result.path.back()].back();
        }
        last_segment = cur_segment;
        if (cur_segment->front() == result.path.back()) {
            result.path.insert(result.path.end(), cur_segment->begin() + 1, cur_segment->end());
        } else {
            result.path.insert(result.path.end(), cur_segment->rbegin() + 1, cur_segment->rend());
        }
    }

    std::cout << "Stepcount = " << step_counter << std::endl;
    std::cout << getValue() << " KByte" << std::endl;

    return result;
}

void QueryManager::unpack(Block* block, BNodePairs& key, std::vector<std::vector<NodeID>> &path_segments) {
    PathMap::accessor acc;
    block->pathmap.find(acc, key);
    Path& path = acc->second;
    acc.release();
    //Path& path = block->pathmap.at(key);
    if (block->subblocks.size()) {
        for (BlockID subblock_id = 0; subblock_id < path.size(); subblock_id++) {
            //TODO only if matching isn't empty/ small optimization, probably doesn't matter
            unpack(block->subblocks[subblock_id], path[subblock_id], path_segments);
            //block->subblocks[subblock_id]->pathmap.at(path[subblock_id]);
        }
        for (Index pos = 0; pos + 1 < path.edges.size(); pos += 2) {
            path_segments.push_back({path.edges[pos], path.edges[pos + 1]});
        }
    } else {
        BaseBlock* level0block = static_cast<BaseBlock*>(block);
        auto& vector = key.get_vector();
        Index pos = 0;
        for (NodeID start = 0; start < vector.size(); start++) {
            NodeID end = vector[start];
            if (end != invalid_node && start < end) {
                ensure(path.edges[pos] == start, "does not work");
                path_segments.push_back({});
                do {
                    path_segments.back().push_back(level0block->input_ids[path.edges[pos]]);
                    pos++;
                } while (path.edges[pos - 1] != end);
            }
        }
    }
}