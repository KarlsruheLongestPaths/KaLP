/******************************************************************************
 * undirected_graph.h
 *
 * Source of KaLP -- Karlsruhe Longest Paths 
 ******************************************************************************
 * Copyright (C) 2017 Kai Fieger
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

#ifndef __LongestPath__UndirectedGraph__
#define __LongestPath__UndirectedGraph__

#include <stdio.h>
#include <vector>
#include <string>

struct EdgeLP {
    int nextVertex;
    int weight;
    
    EdgeLP(int nextVertex, int weight) : nextVertex(nextVertex), weight(weight) {}
    
    bool operator < (const EdgeLP& b) const {
        return (nextVertex < b.nextVertex);
    }
};

struct UndirectedGraph {
    int size() {
        return (int) nodes.size();
    }
   
    int addNode();
    void addEdge(const int from, const int to, int cost);
    void addEdgeDirected(const int from, const int to, int cost);

    std::vector<std::vector<EdgeLP>> nodes =  {};
};


#endif /* defined(__LongestPath__UndirectedGraph__) */
