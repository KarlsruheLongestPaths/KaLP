/******************************************************************************
 * parse_parameters.h 
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

#ifndef PARSE_PARAMETERS_GPJMGSM8
#define PARSE_PARAMETERS_GPJMGSM8

#include <argtable2.h>
#include <regex.h>
#include <string.h>
#include <fstream>
#include <stdio.h>
#include "configuration.h"

int parse_parameters(int argn, char **argv, 
                     Config & config, 
                     std::string & graph_filename) {

        const char *progname = argv[0];

        // Setup argtable parameters.
        struct arg_lit *help                    = arg_lit0(NULL, "help","Print help.");
        struct arg_str *filename                = arg_strn(NULL, NULL, "FILE", 1, 1, "Path to graph file for longest paths.");
        struct arg_int *user_seed               = arg_int0(NULL, "seed", NULL, "Seed to use for the PRNG.");
        struct arg_int *start_vertex            = arg_int1(NULL, "start_vertex", NULL, "Start vertex to use.");
        struct arg_int *subgraph_size           = arg_int1(NULL, "subgraph_size", NULL, "Number of vertices to be contained in the random subgraph.");
        struct arg_int *target_vertex           = arg_int1(NULL, "target_vertex", NULL, "Target vertex to use.");
        struct arg_rex *partition_configuration = arg_rex0(NULL, "partition_configuration", "^(strong|eco|fast)$", "VARIANT", REG_EXTENDED, "Use a configuration for the partitioning tool. (Default: eco) [strong|eco|fast]." );
        struct arg_lit *print_path              = arg_lit0(NULL, "print_path","Printing the solution at the end of the program.");
        struct arg_int *number_of_threads       = arg_int0(NULL, "threads", NULL, "Number of threads that the program can use.");
        struct arg_int *split_steps             = arg_int0(NULL, "steps", NULL, "Number of steps that the algorithm takes to divide the workload");
        struct arg_int *threshold               = arg_int0(NULL, "threshold", NULL, "block has >threshold boundary nodes => solve in parallel");
#ifndef MODE_RANDOMSUBGRAPH
        struct arg_str *output_filename         = arg_str0(NULL, "output_filename", NULL, "Output filename. If specified the vertices of the longest path will be written into that file.");
#else
        struct arg_str *output_filename         = arg_str1(NULL, "output_filename", NULL, "Output filename. File name of the output graph.");
#endif
        struct arg_end *end                     = arg_end(100);

        // Define argtable.
        void* argtable[] = {

                help, filename, 
#ifndef MODE_RANDOMSUBGRAPH
                start_vertex, target_vertex, number_of_threads, split_steps, print_path, partition_configuration, 
#else
                user_seed, subgraph_size,  
#endif 
                
                output_filename,
                end
        };
        // Parse arguments.
        int nerrors = arg_parse(argn, argv, argtable);

        // Catch case that help was requested.
        if (help->count > 0) {
                printf("Usage: %s", progname);
                arg_print_syntax(stdout, argtable, "\n");
                arg_print_glossary(stdout, argtable,"  %-40s %s\n");
                printf("This is the experimental longest paths program.\n");
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1;
        }


        if (nerrors > 0) {
                arg_print_errors(stderr, end, progname);
                printf("Try '%s --help' for more information.\n",progname);
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1; 
        }

        configuration_lp cfg;
        cfg.standard(config);

        if(filename->count > 0) {
                graph_filename = filename->sval[0];
        }

        if(user_seed->count > 0) {
                config.seed = user_seed->ival[0];
        }

        if(subgraph_size->count > 0) {
                config.subgraph_size = subgraph_size->ival[0];
        }

        if(output_filename->count > 0) {
                config.output_filename = output_filename->sval[0];
        }

        if(partition_configuration->count > 0) {
                if(strcmp("strong", partition_configuration->sval[0]) == 0) {
                        config.partition_configuration = PARTITION_CONFIG_STRONG; 
                } else if (strcmp("eco", partition_configuration->sval[0]) == 0) {
                        config.partition_configuration = PARTITION_CONFIG_ECO; 
                } else if (strcmp("fast", partition_configuration->sval[0]) == 0) {
                        config.partition_configuration = PARTITION_CONFIG_FAST; 
                } else {
                        fprintf(stderr, "Invalid partition_configuration variant: \"%s\"\n", partition_configuration->sval[0]);
                        exit(0);
                }
        }

        if(start_vertex->count > 0) {
                config.start_vertex = start_vertex->ival[0];
        }

        if(target_vertex->count > 0) {
                config.target_vertex = target_vertex->ival[0];
        }

        if(number_of_threads->count > 0) {
            config.number_of_threads = number_of_threads->ival[0];
        }

        if(split_steps->count > 0) {
            config.split_steps = split_steps->ival[0];
        }

        if(threshold->count > 0) {
            config.threshold = threshold->ival[0];
        }

        if(print_path->count > 0) {
                config.print_path = true;
        }

        return 0;
}

#endif /* end of include guard: PARSE_PARAMETERS_GPJMGSM8 */
