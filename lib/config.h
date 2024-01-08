/******************************************************************************
 * config.h 
 *
 * Source of KaLP -- Karlsruhe Longest Paths.
 *
 ******************************************************************************
 * Copyright (C) 2017 Christian Schulz <christian.schulz@kit.edu>
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

#ifndef CONFIG_DI1ES4T02
#define CONFIG_DI1ES4T02

#include "definitions.h"

typedef enum {
        PARTITION_CONFIG_STRONG, 
        PARTITION_CONFIG_FAST, 
	PARTITION_CONFIG_ECO 
} PartitionConfiguration;

struct Config
{
        Config() {}

        int seed;

        int number_of_blocks;

        NodeID start_vertex;

        NodeID target_vertex;

        bool print_path;

        int block_size;

        PartitionConfiguration partition_configuration;

        std::string output_filename;

        int subgraph_size;

        unsigned number_of_threads;

        unsigned split_steps;

        unsigned threshold;

        void LogDump(FILE *out) const {
        }
};


#endif /* end of include guard: PARTITION_CONFIG_DI1ES4T02 */
