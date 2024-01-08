/******************************************************************************
 * undirected_graph.cpp 
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


#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "undirected_graph.h"

int UndirectedGraph::addNode() {
        nodes.push_back(std::vector<EdgeLP>());
        return (int)nodes.size() - 1;
}

void UndirectedGraph::addEdgeDirected(const int from, const int to, int cost) {
        nodes[from].push_back(EdgeLP(to, cost));
}

void UndirectedGraph::addEdge(const int from, const int to, int cost) {
        nodes[from].push_back(EdgeLP(to, cost));
        nodes[to].push_back(EdgeLP(from, cost));
}
