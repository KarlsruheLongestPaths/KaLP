//
//  main.cpp
//  LongestPath
//
//  Created by Kai Fieger on 03.05.16.
//  Copyright (c) 2016 KaiFieger. All rights reserved.
//

#include <iostream>
#include <chrono>
#include "UndirectedGraph.h"
#include <time.h>
#include "string.h"
#include "Partitioning.h"
#include "roadmap.h"
#include <unordered_set>
#include <sys/time.h>
#include <chrono>
#include "constants.hpp"
#include <fstream>
#include <sstream>

#include "QueryManager.hpp"
#include "Block.hpp"

#include "tbb/concurrent_hash_map.h"


unsigned number_of_threads = 1;
unsigned split_steps = 0;
unsigned threshold;


    /*
     0  1  2  3
     4  5  6  7
     8  9  10 11
     12 13 14 15

     0  1  0
     2  3  1  2
     0  1  0  1
        2  2  3

     0
     1  2  3  4
     0  1  2  3
              4
    */


void showPartitionOfGrid(UndirectedGraph *graph, std::vector<std::vector<int>> *partitions, std::vector<std::vector<int>> *convertedNodes) {
while (true) {
    int level;
    int number;
    std::cin >> level;
    std::cin >> number;
    for (int i = 0; i <= level; i++) {
        number = (*partitions)[i][number];
    }
    for (int i  = 0; i < convertedNodes->size(); i++) {
        for (int j = 0; j < (*convertedNodes)[i].size(); j++) {
            const int node = (*convertedNodes)[i][j];
            if (node == -1) {
                std::cout << '#';
            } else {
                int cur = node;
                for (int k = 0; k <= level; k++) {
                    cur = (*partitions)[k][cur];
                }
                if (cur == number) {
                    std::cout << 'X';

                } else {
                    std::cout << ' ';
                }
            }
        }
        std::cout << std::endl;
    }

}
}


void printPartition(std::vector<std::vector<int>> &partitions) {
    std::cout << "------------------------------" << std::endl;
    std::cout << "partition:" << std::endl;
    for (int i = 0; i < partitions.size(); i++) {
        std::cout << "c p";
        for (int j = 0; j < partitions[i].size(); j++) {
            std::cout << " " << partitions[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "------------------------------" << std::endl;
}

void run(std::string name, std::string kaHIP, bool writeBack, int blockSize, int times) {
    /*char *workingDir = getenv("PWD");
    char *filePath = (char*)malloc(strlen(workingDir) + name.length() + 1);
    strcpy(filePath, workingDir);
    strcat(filePath, "/");
    strcat(filePath, name.c_str());
    */
    UndirectedGraph *graph = new UndirectedGraph();
    std::vector<std::vector<int>> partitions;
    int s;
    int g;
    graph->createGraph(name, &partitions, &s, &g);
    
    struct timeval start_tv;
    gettimeofday(&start_tv, NULL);
    
    //if partition with KaHip
    //TODO conditions for normal graphs
    if (!kaHIP.empty()) {
        int numberOfParititons = graph->size()/blockSize;
        if (graph->size() % blockSize) {
            numberOfParititons++;
        }
        partitionGraph(graph, &partitions, numberOfParititons, 2, times, kaHIP);
    }
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double elapsed = (tv.tv_sec - start_tv.tv_sec) + (tv.tv_usec - start_tv.tv_usec) / 1000000.0;
    
    
    printPartition(partitions);
    
    if (writeBack) {
        graph->printGraph(name, &partitions, s, g);
        return;
    }

    QueryManager q(graph, partitions, s, g);


    //clock_t start = clock();
    const auto query_start = std::chrono::high_resolution_clock::now();
    Result result = q.run(number_of_threads, split_steps);
    const auto query_end = std::chrono::high_resolution_clock::now();
    const double calcTime = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(query_end - query_start).count();
    //double calcTime = (clock() - start) / (double) CLOCKS_PER_SEC;
    std::cout << "Total: " << calcTime << " sec" << std::endl;
    std::cout << "Length = " << result.weight << std::endl;
    std::cout << "#Edges = " << result.path.size()-1 << std::endl;
    std::cout << "Nodes: ";
    for (int i = 0; i < result.path.size(); i++) {
        std::cout << result.path[i] + 1 << " ";
    }
    std::cout << "\n" << std::endl;
    //free(filePath);
    
    if (kaHIP.empty()) elapsed = 0.0;
    std::cout << "Partitioning: " << elapsed << " sec" << std::endl;
    std::cout << "Overall: " << calcTime + elapsed << " sec" << std::endl;
    
    delete graph;
}

void runGrid(std::string name, std::string kaHIP, bool writeBack, int blockSize, int times) {
    UndirectedGraph *graph = new UndirectedGraph();
    std::vector<std::vector<int>> partitions;
    int s;
    int g;
    std::vector<std::vector<int>> convertedNodes;
    graph->createGridGraph(name, &partitions, &s, &g, &convertedNodes);

    //showPartitionOfGrid(graph, &partitions, &convertedNodes);
    
    struct timeval start_tv;
    gettimeofday(&start_tv, NULL);
    
    
    //if partition with KaHip
    //TODO conditions for grids
    unsigned long nedges = 0;
    for (int i = 0; i < graph->nodes.size(); i++) {
        nedges += graph->nodes[i].size();
    }
    nedges /= 2;
    unsigned long nallPossibleEdges = convertedNodes.size() * convertedNodes[0].size() * 2
                            - convertedNodes.size()
                            - convertedNodes[0].size();
    double percent = (double)nedges/(double)nallPossibleEdges;
    //test with linear interpolation
    //5 nodesPerPartition at percent == 1.0
    //15 at percent == 0.4
    int nodesPerPartition = (int)(15 + (5 - 15) / (1.0 - 0.4) * (percent - 0.4));
    int numberOfParititons = graph->size() / nodesPerPartition;
    if (graph->size() % nodesPerPartition) {
        numberOfParititons++;
    }
    if (!kaHIP.empty()) {
        partitionGraph(graph, &partitions, numberOfParititons, 2, times, kaHIP);
    }
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double elapsed = (tv.tv_sec - start_tv.tv_sec) + (tv.tv_usec - start_tv.tv_usec) / 1000000.0;
    
    printPartition(partitions);
    
    if (writeBack) {
        graph->printGridGraph(name, &partitions, s, g, &convertedNodes);
    }

    QueryManager q(graph, partitions, s, g);
    //clock_t start = clock();
    const auto query_start = std::chrono::high_resolution_clock::now();
    Result result = q.run(number_of_threads, split_steps);
    const auto query_end = std::chrono::high_resolution_clock::now();
    const double calcTime = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(query_end - query_start).count();
    //double calcTime = (clock() - start) / (double) CLOCKS_PER_SEC;
    std::cout << "Total: " << calcTime << " sec" << std::endl;
    std::cout << "Length = " << result.weight << std::endl;
    std::cout << "Path:" << std::endl;

    std::vector<std::vector<unsigned char>> output;
    int startX = 0;
    int startY = 0;
    for (int i = 0; i < convertedNodes.size(); i++) {
        output.push_back(std::vector<unsigned char>(0));
        for (int j = 0; j < convertedNodes[i].size(); j++) {
            const int node = convertedNodes[i][j];
            if (node == -1) {
                output.back().push_back('#');
            } else  if (node == s) {
                startX = i;
                startY = j;
                output.back().push_back(' ');
                //output.back().push_back('@');
            } else  if (node == g) {
                output.back().push_back(' ');
                //output.back().push_back('*');
            } else {
                output.back().push_back(' ');
            }
        }
    }
    int x = startX;
    int y = startY;
    int nx[] = { 0, 0,-1, 1};
    int ny[] = {-1, 1, 0, 0};
    for (int i = 0; i + 1 < result.path.size(); i++) {
        for (int k = 0; k < 4; k++) {
            int newx = x+nx[k];
            int newy = y+ny[k];
            if (-1 < newx && newx < output.size() && -1 < newy && newy < output[0].size()) {
                if (result.path[i + 1] == convertedNodes[newx][newy]) {
                    switch (k) {
                        case 0:
                            output[x][y] = '<';
                            break;
                        case 1:
                            output[x][y] = '>';
                            break;
                        case 2:
                            output[x][y] = '^';
                            break;
                        default:
                            output[x][y] = 'v';
                            break;
                    }
                    x = newx;
                    y = newy;
                    break;
                }
            }
        }
    }
    output[startX][startY] = '@';
    output[x][y] = '*';
    
    for (int i = 0; i < output.size(); i++) {
        for (int j = 0; j < output[i].size(); j++) {
            std::cout << output[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    if (kaHIP.empty()) elapsed = 0.0;
    std::cout << "Partitioning: " << elapsed << " sec" << std::endl;
    std::cout << "Overall: " << calcTime + elapsed << " sec" << std::endl;
    
    
    delete graph;
}

struct RoadNetwork {
    unsigned int roadmapNodecount;
    struct RoadmapNode *roadmapNodes;
    unsigned int start, goal;
    std::unordered_set<unsigned int> nodes;

    RoadNetwork(std::string binfile, std::string filename, std::vector<std::vector<int>> *partitions) : roadmapNodes(NULL) {
        FILE* networkFile = fopen(binfile.c_str(), "r");
        if(networkFile == NULL) {
            std::cerr << "FAILED TO OPEN ROAD NETWORK FILE\n";
            exit(0);
        }
        else if(load_map(networkFile, &roadmapNodes, &roadmapNodecount) != 0) {
            fclose(networkFile);
            std::cerr << "FAILED TO BUILD ROAD NETWORK\n";
            exit(0);
        }
        else {
            std::vector<std::vector<int>> lines;
            std::ifstream infile;
            infile.open(filename, std::ios::in);
            std::string lineString;
            bool eof;
            int stage = 0;
            while (true) {
                eof = std::getline(infile, lineString).eof();
                std::istringstream line(lineString);

                lines.push_back({});

                line >> std::noskipws;
                char c;
                while (line.peek() == 'c' || line.peek() == 'p' || line.peek() == ' ') {
                    line >> c;
                }

                line >> std::skipws;
                int number;
                while (line >> number) {
                    lines.back().push_back(number);
                }

                if (eof) {
                    break;
                }
            }
            infile.close();

            //last line = all nodes of the (sub)graph
            for (int node : lines.back()) {
                nodes.insert(node);
            }
            lines.pop_back();

            //2nd last line = start and goal.
            start = lines.back()[0];
            goal = lines.back()[1];
            lines.pop_back();

            //all lines above the last two = partitioning
            partitions->clear();
            int highestPartition;
            for (int i = 0; i < lines.size(); i++) {
                partitions->push_back({});
                highestPartition = -1;
                for (int j = 0; j < lines[i].size(); j++) {
                    if (lines[i][j] > highestPartition) {
                        highestPartition = lines[i][j];
                    }
                    partitions->back().push_back(lines[i][j]);
                }
            }
            if ((*partitions).size() == 0) {
                (*partitions).push_back({});
                highestPartition = 0;
                (*partitions)[0] = std::vector<int>(nodes.size(), 0);
            }
            if (highestPartition != 0) {
                (*partitions).push_back(std::vector<int>(highestPartition + 1, 0));
            }
            if ((*partitions).back().size() != 1) {
                (*partitions).push_back(std::vector<int>(1, 0));
            }
        }
    }

    ~RoadNetwork() {
        if(roadmapNodes != NULL)
            free_map(roadmapNodes, roadmapNodecount);
    }
};

void runRoad(std::string binfile, std::string instfile, std::string kaHIP, int blockSize, int times) {
    UndirectedGraph graph;
    std::vector<std::vector<int>> partitions;
    RoadNetwork map(binfile, instfile, &partitions);
    std::vector<int> convertTo(map.roadmapNodecount, -1);
    std::vector<int> convertBack;
    for (int i = 0; i < map.roadmapNodecount; i++) {
        if (map.nodes.find(i) != map.nodes.end()) {
            convertTo[i] = graph.addNode();
            convertBack.push_back(i);
        }
    }
    int source;
    int target;
    for (int i = 0; i < map.roadmapNodecount; i++) {
        source = convertTo[i];
        if (source != -1) {
            for (int j = 0; j < map.roadmapNodes[i].narcs; j++) {
                target = convertTo[map.roadmapNodes[i].arcv[j].target];
                if (source < target) {
                    graph.addEdge(source, target, map.roadmapNodes[i].arcv[j].wt);
                }
            }
        }
    }

    struct timeval start_tv;
    gettimeofday(&start_tv, NULL);

    if (!kaHIP.empty()) {
        partitions.clear();
        int numberOfParititons = graph.size()/blockSize;
        if (graph.size() % blockSize) {
            numberOfParititons++;
        }
        partitionGraph(&graph, &partitions, numberOfParititons, 2, times, kaHIP);
    }

    /*
    instfile.pop_back();
    instfile.pop_back();
    instfile.pop_back();
    instfile.pop_back();
    instfile.append("txt");
    graph.printGraph(instfile, &partitions, convertTo[map.start], convertTo[map.goal] );
    return;
    */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double elapsed = (tv.tv_sec - start_tv.tv_sec) + (tv.tv_usec - start_tv.tv_usec) / 1000000.0;

    std::cout << "------------------------------" << std::endl;
    for (int i = 0; i < convertBack.size(); i++) {
        std::cout << convertBack[i] << "=>" << i << "\n";
    }
    printPartition(partitions);

    QueryManager q(&graph, partitions, convertTo[map.start], convertTo[map.goal]);
    //clock_t start = clock();
    const auto query_start = std::chrono::high_resolution_clock::now();
    Result result = q.run(number_of_threads, split_steps);
    const auto query_end = std::chrono::high_resolution_clock::now();
    const double calcTime = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(query_end - query_start).count();
    //double calcTime = (clock() - start) / (double) CLOCKS_PER_SEC;
    std::cout << "Total: " << calcTime << " sec" << std::endl;
    std::cout << "Length = " << result.weight << std::endl;
    std::cout << "#Edges = " << result.path.size()-1 << std::endl;
    std::cout << "Nodes: ";
    for (int i = 0; i < result.path.size(); i++) {
        std::cout << convertBack[result.path[i]]/*path[i] + 1*/ << " ";
    }
    std::cout << "\n" << std::endl;

    if (kaHIP.empty()) elapsed = 0.0;
    std::cout << "Partitioning: " << elapsed << " sec" << std::endl;
    std::cout << "Overall: " << calcTime + elapsed << " sec" << std::endl;
}


int main(int argc, const char * argv[]) {
    threshold = 0;
    if (argc < 2) {
        return 1;
    } else {
        std::string path = "";
        std::string roadfile = "";
        int blockSize = 10;
        std::string kaHIP = "";
        bool writeBack = false;
        int times = 1;
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-inst") == 0) {
                if (++i < argc) path = std::string(argv[i]);
            } else if (strcmp(argv[i], "-net") == 0) {
                if (++i < argc) roadfile = std::string(argv[i]);
            } else if (strcmp(argv[i], "-KAHIP") == 0) {
                if (++i < argc) {
                    kaHIP = argv[i];
                }
            } else if (strcmp(argv[i], "--WRITE_BACK_PARTITION") == 0) {
                writeBack = true;
            } else if (strcmp(argv[i], "-psize") == 0) {
                if (++i < argc) {
                    int temp = (int) strtol(argv[i], NULL, 10);
                    if (temp > 0) {
                        blockSize = temp;
                    }
                }
            } else if (strcmp(argv[i], "-times") == 0) {
                if (++i < argc) {
                    int temp = (int) strtol(argv[i], NULL, 10);
                    if (temp > 0) {
                        times = temp;
                    }
                }
            } else if (strcmp(argv[i], "-threads") == 0) {
                if (++i < argc) {
                    int temp = (int) strtol(argv[i], NULL, 10);
                    if (temp > 0) {
                        number_of_threads = (unsigned)temp;
                    }
                }
            } else if (strcmp(argv[i], "-steps") == 0) {
                if (++i < argc) {
                    int temp = (int) strtol(argv[i], NULL, 10);
                    if (temp >= 0) {
                        split_steps = (unsigned)temp;
                    }
                }
            } else if (strcmp(argv[i], "-threshold") == 0) {
                if (++i < argc) {
                    int temp = (int) strtol(argv[i], NULL, 10);
                    if (temp >= 0) {
                        threshold = (unsigned)temp;
                    }
                }
            } else {
                //std::cout << argv[i] <<std::endl;
                //std::cout << "unrecognized option " << std::endl;
            }
            /*else if(strcmp(argv[i], "-alg") == 0) {
                alg = string(argv[++i]);
            }*/
        }
        if (path.empty()) {
            //std::cout << "print complete usage" << std::endl;
            //don't run
        } else if (kaHIP.empty()) {
            //std::cout << "KAHIP! the algorithm will simply resemble a bruteforce approach << std::endl;
            //still run
        }
        if (path.substr(path.find_last_of(".") + 1) == "grid") {
            runGrid(path, kaHIP, writeBack, blockSize, times);
        } else if (path.substr(path.find_last_of(".") + 1) == "road") {
            runRoad(roadfile, path, kaHIP, blockSize, times);
        } else {
            run(path, kaHIP, writeBack, blockSize, times);
        }
    }
    return 0;
}