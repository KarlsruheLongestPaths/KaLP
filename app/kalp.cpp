/******************************************************************************
 * kalp.h
 *
 * Source of KaLP -- Karlsruhe Longest Paths 
 ******************************************************************************
 * Copyright (C) 2017 Kai Fieger and Christian Schulz
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

#include <iostream>
#include <time.h>
#include <sys/time.h>

#include "QueryManager.hpp"
#include "partitioning.h"
#include "string.h"
#include "undirected_graph.h"
#include "parse_parameters.h"
#include "io/graph_io.h"
#include "tools/timer.h"

int main(int argn, char ** argv) {

        Config config;
        std::string graph_filename;

        int ret_code = parse_parameters(argn, argv, 
                        config, 
                        graph_filename); 

        if(ret_code) {
                return 0;
        }

        graph_io_lp io;
        UndirectedGraph G; 
        timer t;
        std::vector<std::vector<int>> partitions;
        io.readGraph(G,  graph_filename);

        int start_vertex = config.start_vertex == UNDEFINED_NODE ? 0 : config.start_vertex;
        int target_vertex = config.target_vertex == UNDEFINED_NODE ? G.size()-1 : config.target_vertex;

        std::cout <<  "start vertex " <<  start_vertex  << std::endl;
        std::cout <<  "target vertex " <<  target_vertex << std::endl;

        if( start_vertex < 0 || start_vertex > G.size()-1 || target_vertex < 0 || target_vertex > G.size()-1) {
                std::cout <<  "start and target vertex have to be in the interval [0," << G.size()-1 << "]."  << std::endl;
                exit(0);
        }

        int blockSize = config.block_size;
        int number_of_blocks = G.size()/blockSize;
        if (G.size() % blockSize) {
                number_of_blocks++;
        }

        t.restart();
        partitionGraph(&G, config, &partitions, number_of_blocks, 2);

        double elapsed = t.elapsed();
        std::cout << "Time for Partitioning : " << t.elapsed() << " sec" << std::endl;

        t.restart();
        //int length = 0; std::vector<int> longest_path;
        //partitionDfsLongestPath(G, &length, &longest_path, &partitions, start_vertex, target_vertex);


        QueryManager q(&G, partitions, start_vertex, target_vertex);
        Result result = q.run(config.number_of_threads, config.split_steps, config.threshold);
        std::cout << "Time Longest Paths: " << t.elapsed()           << " sec"     << std::endl;
        std::cout << "Total Time "          << t.elapsed() + elapsed << std::endl;
        if( result.weight != 0 ) {
                std::cout << "Length = " << result.weight << std::endl;
                std::cout << "#Edges = " << result.path.size()-1 << std::endl;

                if( config.print_path ) {
                        for( unsigned i = 0; i < result.path.size(); i++) {
                                std::cout <<  result.path[i]  << std::endl;
                        }
                }

                if( config.output_filename.compare("") ) {
                        graph_io_lp io;
                        io.writeVector( result.path, config.output_filename);
                }
        } else {
                std::cout <<  "No path exists."  << std::endl;
        }

        return 0;
}
