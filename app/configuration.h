/******************************************************************************
 * configuration.h 
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


#ifndef CONFIGURATION_3APG5VA7Z
#define CONFIGURATION_3APG5VA7Z

#include "config.h"

class configuration_lp {
        public:
                configuration_lp() {} ;
                virtual ~configuration_lp() {};

                void standard( Config & config );
};


inline void configuration_lp::standard( Config & config ) {
        config.seed              = 0;
        config.start_vertex      = UNDEFINED_NODE;
        config.target_vertex     = UNDEFINED_NODE;
        config.print_path        = false;
        config.number_of_blocks  = 1;
        config.block_size	 = 10;
        config.output_filename   = "";
        config.partition_configuration  = PARTITION_CONFIG_ECO;
        config.subgraph_size     = 100;
        config.number_of_threads = 1;
        config.split_steps = 0;
        config.threshold = 0;
}

#endif /* end of include guard: CONFIGURATION_3APG5VA7Z */
