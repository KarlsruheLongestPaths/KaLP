/******************************************************************************
 * partitioning.h
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

#ifndef __LongestPath__Partitioning__
#define __LongestPath__Partitioning__

#include <stdio.h>
#include <fstream>

#include "string.h"
#include "undirected_graph.h"
#include "config.h"

void partitionGraph(UndirectedGraph *orgGraph, Config & config, std::vector<std::vector<int>> *partitions, 
                    int numberOfPartitions, int nodesPerPartition);

#endif /* defined(__LongestPath__Partitioning__) */
