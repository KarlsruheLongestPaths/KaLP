/******************************************************************************
 * partitioning.cpp 
 *
 * Source of KaLP -- Karlsruhe Longest Paths 
 ******************************************************************************
 * Copyright (C) 2017 Kai Fieger and Christian Schulz
 * 
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
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include "partitioning.h"
#include "config.h"
#include "data_structure/graph_access.h"
#include "extern/KaHIP/app/balance_configuration.h"
#include "extern/KaHIP/app/configuration.h"
#include "graph_partitioner.h"
#include "partition_config.h"

void to_graph_access(UndirectedGraph & G, graph_access & Q) {
        int num_edges = 0;
        for (int i = 0; i < G.size(); i++) {
                num_edges += G.nodes[i].size();
        }

        Q.start_construction(G.size(), num_edges);
        for (int i = 0; i < G.size(); i++) {
                int node = Q.new_node();
                Q.setNodeWeight(node, 1);
                for (unsigned int j = 0; j < G.nodes[i].size(); j++) {
                        EdgeID e = Q.new_edge(node, G.nodes[i][j].nextVertex);
                        Q.setEdgeWeight(e, G.nodes[i][j].weight);
                }
        }
        Q.finish_construction();
}

void perform_partitioning(graph_access & Q, Config & config) {

        std::streambuf* backup = std::cout.rdbuf();
        std::ofstream ofs;
        ofs.open("/dev/null");
        std::cout.rdbuf(ofs.rdbuf()); 

        PartitionConfig partition_config;
        configuration cfg; 
        cfg.eco( partition_config );
        partition_config.imbalance = 3;
        partition_config.epsilon   = 0.03;

        if( config.partition_configuration == PARTITION_CONFIG_STRONG) {
                cfg.strong(partition_config);
                partition_config.imbalance = 10;
                partition_config.epsilon   = 0.1;
        } else if( config.partition_configuration == PARTITION_CONFIG_FAST ) {
                cfg.fast(partition_config);
                partition_config.imbalance = 3;
                partition_config.epsilon   = 0.03;
        }

        partition_config.k         = config.number_of_blocks;
        graph_partitioner partitioner;
        balance_configuration bc;
        bc.configurate_balance( partition_config, Q);

        partitioner.perform_partitioning(partition_config, Q);

        ofs.close();
        std::cout.rdbuf(backup);

}

void partitionGraph(UndirectedGraph *orgGraph, Config & config, std::vector<std::vector<int>> *partitions, 
                    int numberOfPartitions, int nodesPerPartition) {

        UndirectedGraph graph;
        graph.nodes = std::vector<std::vector<EdgeLP>>(orgGraph->size());
        for (int i = 0; i < orgGraph->size(); i++) {
                for (unsigned int j = 0; j < orgGraph->nodes[i].size(); j++) {
                        graph.nodes[i].push_back(orgGraph->nodes[i][j]);
                }
        }

        partitions->clear();
        if (numberOfPartitions == 1) {
                partitions->push_back(std::vector<int>(graph.size(), 0));
        }

        config.number_of_blocks = numberOfPartitions;
        for (int i = 0; numberOfPartitions != 1; i++) {
                graph_access Q;
                to_graph_access(graph, Q);

                perform_partitioning(Q, config);
                partitions->push_back(std::vector<int>(0));

                forall_nodes(Q, node) {
                        (*partitions)[i].push_back(Q.getPartitionIndex(node));
                } endfor

                std::vector<std::vector<int>> connections(numberOfPartitions, std::vector<int>(numberOfPartitions, 0));
                int secondNode;
                for (int firstNode = 0; firstNode < graph.size(); firstNode++) {
                        for (unsigned int j = 0; j < graph.nodes[firstNode].size(); j++) {
                                secondNode = graph.nodes[firstNode][j].nextVertex;
                                if ((*partitions)[i][firstNode] < (*partitions)[i][secondNode]) {
                                        connections[(*partitions)[i][firstNode]][(*partitions)[i][secondNode]] = connections[(*partitions)[i][firstNode]][(*partitions)[i][secondNode]]+1;
                                }
                        }
                }

                graph.nodes.clear();
                graph.nodes = std::vector<std::vector<EdgeLP>>(numberOfPartitions);
                for (unsigned int i = 0; i < connections.size(); i++) {
                        for (unsigned int j = 0; j < connections[i].size(); j++) {
                                if (connections[i][j]) {
                                        graph.addEdge(i, j, connections[i][j]);
                                }
                        }
                }

                numberOfPartitions = graph.size() / nodesPerPartition;
                if (graph.size() % nodesPerPartition) {
                        numberOfPartitions++;
                }

                config.number_of_blocks = numberOfPartitions;
        }

        if (partitions->back().size() != 1) {
                int highest = partitions->back()[0];
                for (unsigned int i = 1; i < partitions->back().size(); i++) {
                        highest = std::max(highest, partitions->back()[i]);
                }
                partitions->push_back(std::vector<int>(highest + 1, 0));
                if (highest != 0) {
                        partitions->push_back(std::vector<int>(1, 0));
                }
        }
}
