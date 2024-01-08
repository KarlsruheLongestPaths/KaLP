/******************************************************************************
 * BlockGraphSearch.cpp
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
 
#include "BlockGraphSearch.hpp"

//#include "PathMapMerger.hpp"

BlockGraphSearch::BlockGraphSearch(Block &block) : block(block), convert_to_subnode(block.number_of_boundary_nodes), convert_to_subblock(block.number_of_boundary_nodes) {
    for (BlockID subblock_id = 0; subblock_id < block.subblocks.size(); subblock_id++) {
        for (NodeID node = 0; node < block.subblocks[subblock_id]->number_of_boundary_nodes; node++) {
            NodeID boundary_node = block.subblocks[subblock_id]->boundary_nodes_in_parent[node];
            if (boundary_node != invalid_node) {
                convert_to_subnode[boundary_node] = node;
                convert_to_subblock[boundary_node] = subblock_id;
            }
        }
    }
}
/*
void merge_pathmaps(PathMap &main, PathMap &other) {
    for (auto& entry : other) {
        if (main.count(entry.first)) {
            if (entry.second.weight > main.at(entry.first).weight) {
                main[entry.first] = entry.second;
            }
        } else {
            main.insert(entry);
        }
    }
}
*/
#include <chrono>

STEP_COUNT BlockGraphSearch::run(unsigned number_of_threads, unsigned split_steps, unsigned threshold) {
    step_counter = 0;

    std::vector<BGSearchBranch> basic_branches;
    for (NodeID node = 0; node + 1 < block.number_of_boundary_nodes; node++) {
        basic_branches.push_back(BGSearchBranch(*this, block, &block.pathmap));
        basic_branches.back().set_branch(node);
    }
    if (block.number_of_boundary_nodes > 1) {
        step_counter += basic_branches[0].init_map(&block.pathmap);
    } else {
        step_counter += BGSearchBranch(*this, block, &block.pathmap).init_map(&block.pathmap);
    }

    if (number_of_threads == 1) { //serial
        unsigned number_sub_boundary_nodes = 0;
        for (auto subblock : block.subblocks) {
            number_sub_boundary_nodes += subblock->number_of_boundary_nodes;
        }
        std::cout << "BLOCK with " << block.number_of_boundary_nodes << " boundary nodes, " << number_sub_boundary_nodes << " sub boundary nodes" << std::endl;
                  /*for (auto& branch : basic_branches) {
                      branch.run(&block.pathmap);
                  }*/
        std::vector<double> overall(number_of_threads, 0.0f);
        /*
        std::vector<PathMap*> maps(number_of_threads);
        maps[0] = &block.pathmap;
        for (Index i = 1; i < number_of_threads; i++) {
            maps[i] = new PathMap();
        }
        */
        const auto split_start = std::chrono::high_resolution_clock::now();

        std::vector<BGSearchBranch> branch_pool;
        for (auto& branch : basic_branches) {
            step_counter += branch.split_work(&branch_pool, split_steps);
        }
        unsigned branches = branch_pool.size();

        const auto split_end = std::chrono::high_resolution_clock::now();
        const double split_time = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(split_end - split_start).count();
        std::cout << "DIVIDE " << split_time << " sec" << std::endl;
        std::cout << "#branches " << branches << std::endl;

        const auto search_start = std::chrono::high_resolution_clock::now();

        for (Index i = 0; i < branches; i++) {
            const auto query_s = std::chrono::high_resolution_clock::now();
			
			BGSearchBranch this_branch = branch_pool[i];
            step_counter += this_branch.run(&block.pathmap/*maps[i % number_of_threads]*/);
            //branch_pool[i].run(maps[i % number_of_threads]);
            const auto query_e = std::chrono::high_resolution_clock::now();
            const double cTime = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(query_e - query_s).count();
            overall[i % number_of_threads] += cTime;
            if (cTime > 15) { std::cout << cTime << std::endl; }
        }

        std::cout << "THREADS" << std::endl;
        for (int i = 0; i < number_of_threads; ++i) {
            std::cout << "Thread " << i  << " took " << overall[i] << " sec" << std::endl;
        }

        const auto search_end = std::chrono::high_resolution_clock::now();
        const double search_time = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(search_end - search_start).count();
        std::cout << "SEARCH " << search_time << " sec" << std::endl;
/*
        const auto merge_start = std::chrono::high_resolution_clock::now();

        Index power_of_two = 1;
        while (power_of_two < maps.size()) {
            for (Index i = 0; i < number_of_threads - power_of_two; i += 2 * power_of_two) {
                //TODO better merge
                //PathMapMerger merger(block.number_of_boundary_nodes, block.connected_by_multiple_boundary_edges);
                //merger.merge(maps[i], maps[i + power_of_two]);
                merge_pathmaps(*maps[i], *maps[i + power_of_two]);
            }

            power_of_two <<= 1;
        }

        const auto merge_end = std::chrono::high_resolution_clock::now();
        const double merge_time = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(merge_end - merge_start).count();
        std::cout << "MERGE " << merge_time << " sec" << std::endl;
*/
        double block_time = split_time + search_time;// + merge_time;
        std::cout << "=> BLOCK took " << block_time << " sec" << std::endl;
        std::cout << "=> Percentages "
                  << " divide " << split_time/block_time
                  << " search " << search_time/block_time
                  /*<< " merge " << merge_time/block_time*/ << std::endl;

        /*
        for (Index i = 1; i < number_of_threads; i++) {
            delete maps[i];
        }
        */
    } else { //parallel
        unsigned number_sub_boundary_nodes = 0;
        for (auto subblock : block.subblocks) {
            number_sub_boundary_nodes += subblock->number_of_boundary_nodes;
        }
        std::cout << "BLOCK with " << block.number_of_boundary_nodes << " boundary nodes, " << number_sub_boundary_nodes << " sub boundary nodes" << std::endl;
        std::vector<double> overall(number_of_threads, 0.0f);
        /*
        std::vector<PathMap*> maps(number_of_threads);
        maps[0] = &block.pathmap;
        for (Index i = 1; i < number_of_threads; i++) {
            maps[i] = new PathMap();
        }
        */

        const auto split_start = std::chrono::high_resolution_clock::now();

        std::vector<BGSearchBranch> branch_pool;
        for (auto& branch : basic_branches) {
            step_counter += branch.split_work(&branch_pool, split_steps);
        }
        unsigned branches = branch_pool.size();

        const auto split_end = std::chrono::high_resolution_clock::now();
        const double split_time = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(split_end - split_start).count();
        std::cout << "DIVIDE " << split_time << " sec" << std::endl;
        std::cout << "#branches " << branches << std::endl;

        const auto search_start = std::chrono::high_resolution_clock::now();

        std::vector<STEP_COUNT> thread_step_counter(number_of_threads, 0);

        omp_set_num_threads(number_of_threads);
        #pragma omp parallel for schedule(dynamic, 1)
        for (Index i = 0; i < branches; i++) {
            const auto query_s = std::chrono::high_resolution_clock::now();
            int tid = omp_get_thread_num();
			BGSearchBranch this_branch = branch_pool[i];
            //TODO what
            thread_step_counter[tid] += this_branch.run(&block.pathmap/*maps[tid]*/);
			//branch_pool[i].run(maps[tid]);
            const auto query_e = std::chrono::high_resolution_clock::now();
            const double cTime = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(query_e - query_s).count();
            overall[tid] += cTime;
            if (cTime > 15) { std::cout << cTime << std::endl; }
        }

        for (STEP_COUNT counter : thread_step_counter) {
            step_counter += counter;
        }

        std::cout << "THREADS" << std::endl;
        for (int i = 0; i < number_of_threads; ++i) {
            std::cout << "Thread " << i  << " took " << overall[i] << " sec" << std::endl;
        }

        const auto search_end = std::chrono::high_resolution_clock::now();
        const double search_time = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(search_end - search_start).count();
        std::cout << "SEARCH " << search_time << " sec" << std::endl;
/*
        const auto merge_start = std::chrono::high_resolution_clock::now();

        Index power_of_two = 1;
        while (power_of_two < maps.size()) {
            #pragma omp parallel for schedule(dynamic, 1)
            for (Index i = 0; i < number_of_threads - power_of_two; i += 2 * power_of_two) {
                //TODO better merge
                //PathMapMerger merger(block.number_of_boundary_nodes, block.connected_by_multiple_boundary_edges);
                //merger.merge(maps[i], maps[i + power_of_two]);
                merge_pathmaps(*maps[i], *maps[i + power_of_two]);
            }

            power_of_two <<= 1;
        }

        const auto merge_end = std::chrono::high_resolution_clock::now();
        const double merge_time = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(merge_end - merge_start).count();
        std::cout << "Merging took " << merge_time << " sec" << std::endl;
*/
        double block_time = split_time + search_time;// + merge_time;
        std::cout << "=> BLOCK took " << block_time << " sec" << std::endl;
        std::cout << "=> Percentages "
                  << " divide " << split_time/block_time
                  << " search " << search_time/block_time
                  /*<< " merge " << merge_time/block_time*/ << std::endl;

        /*
        for (Index i = 1; i < number_of_threads; i++) {
            delete maps[i];
        }
        */

    }

    return step_counter;
}

BGSearchBranch::BGSearchBranch(BlockGraphSearch &cur_search, Block &block, PathMap* result_map) : block(block), result_map(result_map), cur_matching(block.number_of_boundary_nodes), marked(block.subblocks.size()), convert_to_subnode(cur_search.convert_to_subnode), convert_to_subblock(cur_search.convert_to_subblock) {
    std::vector<unsigned> number_of_nodes_per_subblock(0);
    for (BlockID subblock_id = 0; subblock_id < marked.size(); subblock_id++) {
        marked[subblock_id] = std::vector<bool>(block.subblocks[subblock_id]->number_of_boundary_nodes);
        number_of_nodes_per_subblock.push_back(marked[subblock_id].size());
    }
    possible_excluded_nodes.reserve(block.number_of_boundary_nodes);
    cur_path = Path(number_of_nodes_per_subblock);
}

void BGSearchBranch::set_branch(NodeID cur_start_node) {
    this->cur_start_node = cur_start_node;
    auto con = convert(cur_start_node);
    this->branch_start_block = con.first;
    this->branch_start_node = con.second;
    this->branch_switch_subblock = false;
}

STEP_COUNT BGSearchBranch::run(PathMap* result_map) {
    step_counter = 0;
    this->result_map = result_map;
    search(branch_start_block, branch_start_node, branch_switch_subblock);
    return step_counter;
}

void BGSearchBranch::search(BlockID subblock_id, NodeID node, bool switch_subblock) {
    step_counter++;
    marked[subblock_id][node] = true;
    Block* cur_subblock = block.subblocks[subblock_id];

    if (cur_subblock->boundary_nodes_in_parent[node] != invalid_node) { //if node is boundary node
        if (cur_start_node < cur_subblock->boundary_nodes_in_parent[node]) {
            //end current path
            NodeID start = cur_start_node;
            cur_matching.insert(start, cur_subblock->boundary_nodes_in_parent[node]);

			NodeID last_unmarked_boundary_node = block.number_of_boundary_nodes - 1;
			for (; last_unmarked_boundary_node > start; last_unmarked_boundary_node--) {
				auto con = convert(last_unmarked_boundary_node);
                if (marked[con.first][con.second] == false) {
                    break;
                }
			}

            if (switch_subblock) {
                update_solutions();

                //start new paths (don't start one from last_unmarked_boundary_node as it can never be completed)
                for (NodeID next_start_node = start + 1; next_start_node < last_unmarked_boundary_node; next_start_node++) {
                    auto con = convert(next_start_node);
                    if (marked[con.first][con.second] == false) {
                        cur_start_node = next_start_node;
                        search(con.first, con.second, false);
                    }
                }
            } else {
                //last edge was a boundary edge => exclude node in the current subblock
                cur_path[subblock_id].insert(node);

                if (cur_subblock->pathmap.count(cur_path[subblock_id])) {
                    update_solutions();

                    //start new paths (don't start one from last_unmarked_boundary_node as it can never be completed)
                    for (NodeID next_start_node = start + 1; next_start_node < last_unmarked_boundary_node; next_start_node++) {
						auto con = convert(next_start_node);
                        if (marked[con.first][con.second] == false) {
                            cur_start_node = next_start_node;
                            search(con.first, con.second, false);
                        }
                    }
                }

                cur_path[subblock_id].remove(node);
            }

            //return to current path
            cur_matching.remove(start, cur_subblock->boundary_nodes_in_parent[node]);
            cur_start_node = start;

            //no boundary node left in which the current path can finish => stop traversing the current path
			if (start == last_unmarked_boundary_node) {
				marked[subblock_id][node] = false;
				return;
			}
        }
    }
    if (switch_subblock) { //last edge was a clique edge
        //traverse boundary edges
        cur_path.edges.push_back(cur_subblock->boundary_node_to_input_ids[node]);
        for (auto& edge : cur_subblock->boundary_edges(node)) {
            if (marked[edge.target_block][edge.target_node] == false) {
                cur_path.weight += edge.weight;
                cur_path.edges.push_back(block.subblocks[edge.target_block]->boundary_node_to_input_ids[edge.target_node]);
                search(edge.target_block, edge.target_node, false);
                cur_path.edges.pop_back();
                cur_path.weight -= edge.weight;
            }
        }
        cur_path.edges.pop_back();
    } else {
        //traverse boundary edge//&& cur_subblock->pathmap.count(cur_path[subblock_id])
        cur_path[subblock_id].insert(node);
        cur_path.edges.push_back(cur_subblock->boundary_node_to_input_ids[node]);
        for (auto& edge : cur_subblock->boundary_edges(node)) {
            if (marked[edge.target_block][edge.target_node] == false
                && cur_subblock->pathmap.count(cur_path[subblock_id])) {
                cur_path.weight += edge.weight;
                cur_path.edges.push_back(block.subblocks[edge.target_block]->boundary_node_to_input_ids[edge.target_node]);
                search(edge.target_block, edge.target_node, false);
                cur_path.edges.pop_back();
                cur_path.weight -= edge.weight;
            }
        }
        cur_path.edges.pop_back();
        cur_path[subblock_id].remove(node);

        //traverse edges through the current clique
        for (auto clique_node : cur_subblock->boundary_nodes()) {
            if (marked[subblock_id][clique_node] == false) { //also filters out clique_node == node;
                cur_path[subblock_id].insert(node, clique_node);
                if (cur_subblock->pathmap.count(cur_path[subblock_id])) {
                    search(subblock_id, clique_node, true);
                }
                cur_path[subblock_id].remove(node, clique_node);
            }
        }

    }
    marked[subblock_id][node] = false;
}

STEP_COUNT BGSearchBranch::split_work(std::vector<BGSearchBranch> *search_branches, unsigned steps) {
    step_counter = 0;
    search(branch_start_block, branch_start_node, branch_switch_subblock, search_branches, steps);
    return step_counter;
}

void BGSearchBranch::search(BlockID subblock_id, NodeID node, bool switch_subblock, std::vector<BGSearchBranch> *search_branches, unsigned steps) {
    if (steps == 0) {
        branch_start_block = subblock_id;
        branch_start_node = node;
        branch_switch_subblock = switch_subblock;
        search_branches->push_back(*this);
        return;
    }
    step_counter++; //TODO put before if or after?
    marked[subblock_id][node] = true;
    Block* cur_subblock = block.subblocks[subblock_id];

    if (cur_subblock->boundary_nodes_in_parent[node] != invalid_node) { //if node is boundary node
        if (cur_start_node < cur_subblock->boundary_nodes_in_parent[node]) {
            //end current path
            NodeID start = cur_start_node;
            cur_matching.insert(start, cur_subblock->boundary_nodes_in_parent[node]);

			NodeID last_unmarked_boundary_node = block.number_of_boundary_nodes - 1;
			for (; last_unmarked_boundary_node > start; last_unmarked_boundary_node--) {
				auto con = convert(last_unmarked_boundary_node);
                if (marked[con.first][con.second] == false) {
                    break;
                }
			}

            if (switch_subblock) {
                update_solutions();

                //start new paths (don't start one from last_unmarked_boundary_node as it can never be completed)
                for (NodeID next_start_node = start + 1; next_start_node < last_unmarked_boundary_node; next_start_node++) {
                    auto con = convert(next_start_node);
                    if (marked[con.first][con.second] == false) {
                        cur_start_node = next_start_node;
                        search(con.first, con.second, false, search_branches, steps-1);
                    }
                }
			} else {
                //last edge was a boundary edge => exclude node in the current subblock
                cur_path[subblock_id].insert(node);

                if (cur_subblock->pathmap.count(cur_path[subblock_id])) {
                    update_solutions();

                    //start new paths (don't start one from last_unmarked_boundary_node as it can never be completed)
                    for (NodeID next_start_node = start + 1; next_start_node < last_unmarked_boundary_node; next_start_node++) {
						auto con = convert(next_start_node);
                        if (marked[con.first][con.second] == false) {
                            cur_start_node = next_start_node;
                            search(con.first, con.second, false, search_branches, steps-1);
                        }
                    }
			    }

                cur_path[subblock_id].remove(node);
            }

            //return to current path
            cur_matching.remove(start, cur_subblock->boundary_nodes_in_parent[node]);
            cur_start_node = start;

            //no boundary node left in which the current path can finish => stop traversing the current path
			if (start == last_unmarked_boundary_node) {
				marked[subblock_id][node] = false;
				return;
			}
        }
    }
    if (switch_subblock) { //last edge was a clique edge
        //traverse boundary edges
        cur_path.edges.push_back(cur_subblock->boundary_node_to_input_ids[node]);
        for (auto& edge : cur_subblock->boundary_edges(node)) {
            if (marked[edge.target_block][edge.target_node] == false) {
                cur_path.weight += edge.weight;
                cur_path.edges.push_back(block.subblocks[edge.target_block]->boundary_node_to_input_ids[edge.target_node]);
                search(edge.target_block, edge.target_node, false, search_branches, steps-1);
                cur_path.edges.pop_back();
                cur_path.weight -= edge.weight;
            }
        }
        cur_path.edges.pop_back();
    } else {
        //traverse boundary edge//&& cur_subblock->pathmap.count(cur_path[subblock_id])
        cur_path[subblock_id].insert(node);
        cur_path.edges.push_back(cur_subblock->boundary_node_to_input_ids[node]);
        for (auto& edge : cur_subblock->boundary_edges(node)) {
            if (marked[edge.target_block][edge.target_node] == false
                && cur_subblock->pathmap.count(cur_path[subblock_id])) {
                cur_path.weight += edge.weight;
                cur_path.edges.push_back(block.subblocks[edge.target_block]->boundary_node_to_input_ids[edge.target_node]);
                search(edge.target_block, edge.target_node, false, search_branches, steps-1);
                cur_path.edges.pop_back();
                cur_path.weight -= edge.weight;
            }
        }
        cur_path.edges.pop_back();
        cur_path[subblock_id].remove(node);

        //traverse edges through the current clique
        for (auto clique_node : cur_subblock->boundary_nodes()) {
            if (marked[subblock_id][clique_node] == false) { //also filters out clique_node == node;
                cur_path[subblock_id].insert(node, clique_node);
                if (cur_subblock->pathmap.count(cur_path[subblock_id])) {
                    search(subblock_id, clique_node, true, search_branches, steps-1);
                }
                cur_path[subblock_id].remove(node, clique_node);
            }
        }

    }
    marked[subblock_id][node] = false;
}

void BGSearchBranch::update_solutions() {
    possible_excluded_nodes.clear();
    for (NodeID node = 0; node < block.number_of_boundary_nodes; node++) {
        auto con = convert(node);
        if (marked[con.first][con.second] == false && block.connected_by_multiple_boundary_edges[node] != invalid_node) {
            possible_excluded_nodes.push_back(node);
        }
    }

    Weight clique_weight = 0;
    for (BlockID subblock_id = 0; subblock_id < block.subblocks.size(); subblock_id++) {
        clique_weight += block.subblocks[subblock_id]->pathmap.get_weight(cur_path[subblock_id]);
    }
    cur_path.weight += clique_weight;
    update_solutions_help(0);
    cur_path.weight -= clique_weight;
}

void BGSearchBranch::update_solutions_help(Index pos) {
    step_counter++;
    result_map->insert_or_update2(cur_matching, cur_path);

    for (int i = pos; i < possible_excluded_nodes.size(); i++) {
        auto con = convert(possible_excluded_nodes[i]);
        cur_path[con.first].insert(con.second);

        if (block.subblocks[con.first]->pathmap.count(cur_path[con.first])) {
            cur_matching.insert(possible_excluded_nodes[i]);

            Weight new_weight = block.subblocks[con.first]->pathmap.get_weight(cur_path[con.first]);
            cur_path[con.first].remove(con.second);
            Weight old_weight = block.subblocks[con.first]->pathmap.get_weight(cur_path[con.first]);
            cur_path[con.first].insert(con.second);

            cur_path.weight = cur_path.weight - old_weight + new_weight;
            update_solutions_help(i+1);
            cur_path.weight = cur_path.weight - new_weight + old_weight;

            cur_matching.remove(possible_excluded_nodes[i]);
        }

        cur_path[con.first].remove(con.second);
    }
}