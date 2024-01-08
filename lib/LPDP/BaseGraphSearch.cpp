/******************************************************************************
 * BaseGraphSearch.cpp
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
 
#include "BaseGraphSearch.hpp"

BaseGraphSearch::BaseGraphSearch(BaseBlock &block) : block(block) {}

#include <chrono>

STEP_COUNT BaseGraphSearch::run(unsigned number_of_threads, unsigned split_steps, unsigned threshold) {
    step_counter = 0;

    std::vector<SearchBranch> basic_branches;
    for (NodeID node = 0; node + 1 < block.number_of_boundary_nodes; node++) {
        basic_branches.push_back(SearchBranch(block, &block.pathmap));
        basic_branches.back().set_branch(node);
    }
    if (block.number_of_boundary_nodes > 1) {
        step_counter += basic_branches[0].init_map(&block.pathmap);
    } else {
        step_counter += SearchBranch(block, &block.pathmap).init_map(&block.pathmap);
    }

    if (number_of_threads == 1) { //serial
        unsigned number_of_nodes = block.graph_first_out.size() - 1;
        std::cout << "BLOCK with " << block.number_of_boundary_nodes << " boundary nodes, " << number_of_nodes << " nodes" << std::endl;

        std::vector<double> overall(number_of_threads, 0.0f);

        const auto split_start = std::chrono::high_resolution_clock::now();

        std::vector<SearchBranch> branch_pool;
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

            SearchBranch this_branch = branch_pool[i];
            step_counter += this_branch.run(&block.pathmap, false);
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

        double block_time = split_time + search_time;// + merge_time;
        std::cout << "=> BLOCK took " << block_time << " sec" << std::endl;
        std::cout << "=> Percentages "
                  << " divide " << split_time/block_time
                  << " search " << search_time/block_time
                  /*<< " merge " << merge_time/block_time*/ << std::endl;

    } else { //parallel
        unsigned number_of_nodes = block.graph_first_out.size() - 1;
        std::cout << "BLOCK with " << block.number_of_boundary_nodes << " boundary nodes, " << number_of_nodes << " sub boundary nodes" << std::endl;

        std::vector<double> overall(number_of_threads, 0.0f);

        const auto split_start = std::chrono::high_resolution_clock::now();

        std::vector<SearchBranch> branch_pool;
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
            SearchBranch this_branch = branch_pool[i];
            thread_step_counter[tid] += this_branch.run(&block.pathmap, true);
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

        double block_time = split_time + search_time;// + merge_time;
        std::cout << "=> BLOCK took " << block_time << " sec" << std::endl;
        std::cout << "=> Percentages "
                  << " divide " << split_time/block_time
                  << " search " << search_time/block_time
                  /*<< " merge " << merge_time/block_time*/ << std::endl;
    }

    return step_counter;
}

SearchBranch::SearchBranch(BaseBlock &block, PathMap* result_map) : block(block), result_map(result_map), cur_matching(block.number_of_boundary_nodes), marked(block.graph_first_out.size() - 1) {
    cur_path.reserve(block.graph_first_out.size() - 1);
    possible_excluded_nodes.reserve(block.number_of_boundary_nodes);
}

void SearchBranch::set_branch(NodeID cur_start_node) {
    this->cur_start_node = cur_start_node;
    this->branch_start_node = cur_start_node;
}

STEP_COUNT SearchBranch::run(PathMap* result_map, bool parallel) {
    step_counter = 0;
    this->result_map = result_map;
    if (parallel) {
        //std::cout << "p" << std::endl;
        update_solutions_help = &SearchBranch::par_update_solutions;
    } else {
        //std::cout << "s" << std::endl;
        update_solutions_help = &SearchBranch::ser_update_solutions;
    }
    search(branch_start_node);
    return step_counter;
}

void SearchBranch::search(NodeID node) {
    step_counter++;
    marked[node] = true;
    cur_path.edges.push_back(node);
    if (node < block.number_of_boundary_nodes) { //if node is boundary node
        if (cur_start_node < node) {
            //end current path
            NodeID start = cur_start_node;
            cur_matching.insert(start, node);

            update_solutions();

            NodeID last_unmarked_boundary_node = block.number_of_boundary_nodes - 1;
            for (; last_unmarked_boundary_node > start; last_unmarked_boundary_node--) {
                if (marked[last_unmarked_boundary_node] == false) {
                    break;
                }
            }

            //start new paths (don't start one from last_unmarked_boundary_node as it can never be completed)
            for (NodeID next_start_node = start + 1; next_start_node < last_unmarked_boundary_node; next_start_node++) {
                if (marked[next_start_node] == false) {
                    cur_start_node = next_start_node;
                    search(next_start_node);
                }
            }

            //return to current path
            cur_matching.remove(start, node);
            cur_start_node = start;

            //no boundary node left in which the current path can finish => stop traversing the current path
            if (start == last_unmarked_boundary_node) {
                cur_path.edges.pop_back();
                marked[node] = false;
                return;
            }
        }
    }
    for (NodeID edge = block.graph_first_out[node]; edge < block.graph_first_out[node+1]; edge++) {
        if (marked[block.graph_target_node[edge]] == false) {
            cur_path.weight += block.graph_weight[edge];
            search(block.graph_target_node[edge]);
            cur_path.weight -= block.graph_weight[edge];
        }
    }
    cur_path.edges.pop_back();
    marked[node] = false;
}


STEP_COUNT SearchBranch::split_work(std::vector<SearchBranch> *search_branches, unsigned steps) {
    step_counter = 0;
    search(branch_start_node, search_branches, steps);
    return step_counter;
}


void SearchBranch::search(NodeID node, std::vector<SearchBranch> *search_branches, unsigned steps) {
    if (steps == 0) {
        branch_start_node = node;
        search_branches->push_back(*this);
        return;
    }
    step_counter++; //TODO put before if or after?
    marked[node] = true;
    cur_path.edges.push_back(node);
    if (node < block.number_of_boundary_nodes) { //if node is boundary node
        if (cur_start_node < node) {
            //end current path
            NodeID start = cur_start_node;
            cur_matching.insert(start, node);

            update_solutions();

            NodeID last_unmarked_boundary_node = block.number_of_boundary_nodes - 1;
            for (; last_unmarked_boundary_node > start; last_unmarked_boundary_node--) {
                if (marked[last_unmarked_boundary_node] == false) {
                    break;
                }
            }

            //start new paths (don't start one from last_unmarked_boundary_node as it can never be completed)
            for (NodeID next_start_node = start + 1; next_start_node < last_unmarked_boundary_node; next_start_node++) {
                if (marked[next_start_node] == false) {
                    cur_start_node = next_start_node;
                    search(next_start_node, search_branches, steps-1);
                }
            }

            //return to current path
            cur_matching.remove(start, node);
            cur_start_node = start;

            //no boundary node left in which the current path can finish => stop traversing the current path
            if (start == last_unmarked_boundary_node) {
                cur_path.edges.pop_back();
                marked[node] = false;
                return;
            }
        }
    }
    for (NodeID edge = block.graph_first_out[node]; edge < block.graph_first_out[node+1]; edge++) {
        if (marked[block.graph_target_node[edge]] == false) {
            cur_path.weight += block.graph_weight[edge];
            search(block.graph_target_node[edge], search_branches, steps-1);
            cur_path.weight -= block.graph_weight[edge];
        }
    }
    cur_path.edges.pop_back();
    marked[node] = false;
}

void SearchBranch::update_solutions() {
    possible_excluded_nodes.clear();
    for (NodeID node = 0; node < block.number_of_boundary_nodes; node++) {
        if (marked[node] == false && block.connected_by_multiple_boundary_edges[node] != invalid_node) {
            possible_excluded_nodes.push_back(node);
        }
    }
    (this->*update_solutions_help)(0);
}

void SearchBranch::ser_update_solutions(Index pos) {
    step_counter++;
    if (block.pathmap.count(cur_matching)) {
        if (cur_path.weight > block.pathmap.get_weight(cur_matching)) {
            PathMap::accessor acc;
            block.pathmap.find(acc, cur_matching);
            acc->second = cur_path;
            acc.release();
            for (int i = pos; i < possible_excluded_nodes.size(); i++) {
                cur_matching.insert(possible_excluded_nodes[i]);
                ser_update_solutions_no_test(i+1);
                cur_matching.remove(possible_excluded_nodes[i]);
            }
        } else {
            for (int i = pos; i < possible_excluded_nodes.size(); i++) {
                cur_matching.insert(possible_excluded_nodes[i]);
                ser_update_solutions(i+1);
                cur_matching.remove(possible_excluded_nodes[i]);
            }
        }
    } else {
        block.pathmap.insert(std::pair<BNodePairs, Path>(cur_matching, cur_path));
        for (Index i = pos; i < possible_excluded_nodes.size(); i++) {
            cur_matching.insert(possible_excluded_nodes[i]);
            ser_update_solutions_no_test(i+1);
            cur_matching.remove(possible_excluded_nodes[i]);
        }
    }
}

void SearchBranch::ser_update_solutions_no_test(Index pos) {
    step_counter++;
    PathMap::accessor acc;
    if (!block.pathmap.insert(acc, std::make_pair(cur_matching, cur_path))) {
        acc->second = cur_path;
    }
    acc.release();
    for (Index i = pos; i < possible_excluded_nodes.size(); i++) {
        cur_matching.insert(possible_excluded_nodes[i]);
        ser_update_solutions_no_test(i+1);
        cur_matching.remove(possible_excluded_nodes[i]);
    }
}

void SearchBranch::par_update_solutions(Index pos) {
    step_counter++;
    result_map->insert_or_update2(cur_matching, cur_path);

    for (int i = pos; i < possible_excluded_nodes.size(); i++) {
        cur_matching.insert(possible_excluded_nodes[i]);
        par_update_solutions(i+1);
        cur_matching.remove(possible_excluded_nodes[i]);
    }
}

/* ser_update_solutions & ser_update_solutions_no_test idea cannot be used in the parallel version
 * example:
 * current solutions:{0,1} weight 1   AND   {0,1}{2,2} weight 1
 * Thread 1 found a solution with weight 2 for both
 * Thread 2 found a solution with weight 3 for both
 * Thread 1 updates {0,1} with 2
 * Thread 2 updates {0,1} with 3
 * Thread 2 continues to update {0,1}{2,2} with 3 without testing
 * Thread 1 continues to update {0,1}{2,2} with 2 without testing
 * !!! {0,1} weight 3   AND   {0,1}{2,2} weight 2!!!
 * => because of this race condition you have to check evertime (insert_or_update2() does this)
 */