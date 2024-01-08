/******************************************************************************
 * kalp.h
 *
 * Source of KaLP -- Karlsruhe Longest Paths 
 ******************************************************************************
 * Copyright (C) 2017 Christian Schulz
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

#include "parse_parameters.h"
#include "io/graph_io.h"
#include "extern/KaHIP/lib/io/graph_io.h"
#include "extern/KaHIP/lib/tools/graph_extractor.h"
#include "tools/timer.h"
#include "tools/random_functions.h"
#include "data_structure/graph_access.h"

int main(int argn, char ** argv) {

        Config config;
        std::string graph_filename;

        int ret_code = parse_parameters(argn, argv, 
                        config, 
                        graph_filename); 

        if(ret_code) {
                return 0;
        }

        graph_access G;     

        timer t;
        graph_io::readGraphWeighted(G, graph_filename);
        std::cout << "io time: " << t.elapsed()  << std::endl;

        srand(config.seed);
        random_functions::setSeed(config.seed);

        forall_nodes(G, node) {
                G.setPartitionIndex(node, 0);
        } endfor
        
        std::vector< bool > touched(G.number_of_nodes(), false);
        std::queue< NodeID > bfsqueue;
        NodeID source = random_functions::nextInt(0, G.number_of_nodes()-1);
        bfsqueue.push(source);
        touched[source] = true;
        int touched_nodes  = 0;
        
        while(!bfsqueue.empty()) {
                NodeID node = bfsqueue.front();
                bfsqueue.pop(); 
                G.setPartitionIndex(node, 1);
                touched_nodes++;

                if( touched_nodes > config.subgraph_size ) 
                        break;
                
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if(!touched[target]) {
                                touched[target] = true;
                                bfsqueue.push(target);
                        }
                } endfor
        }

        std::vector<NodeID> mapping;
        graph_access Q; // subgraph
        PartitionID block = 1;
        graph_extractor ge;
        ge.extract_block(G, Q, block, mapping);

        if( config.output_filename.compare("") ) {
                graph_io::writeGraphWeighted( Q, config.output_filename);
        }

        return 0;
}
