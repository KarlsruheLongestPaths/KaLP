/******************************************************************************
 * generate_maze.cpp
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h> 
#include <unordered_set>
#include <vector>
#include <argtable2.h>
#include <regex.h>

#include "io/graph_io.h"

using namespace std;

void dumpInstance(FILE* out, const vector<int> grid, unsigned int width, unsigned int height) {
        for(unsigned int j = 0; j < grid.size(); j++) {
                if(j > 0 && j % width == 0) {
                        fprintf(out, "\n");
                }
                switch(grid[j]) {
                        case 1: {
                                        fprintf(out, "@"); break;
                                }
                        case 2: {
                                        fprintf(out, "*"); break;
                                }
                        case -1: {
                                        fprintf(out, "#"); break;
                                 }
                        default: {
                                        fprintf(out, " "); break;
                                 }
                }
        }
        fprintf(out, "\n");
}

vector<unsigned int> expand(int n, int width, int height) {
        vector<unsigned int> children;
        int x = n % width;
        int y = n / width;
        if(x + 1 < width) children.push_back(n+1);
        if(x - 1 >= 0) children.push_back(n-1);
        if(y + 1 < height) children.push_back(n+width);
        if(y - 1 >= 0) children.push_back(n-width);
        return children;
}

bool isSolvableFourway(const vector<int> grid, unsigned int width, unsigned int height) {
        vector<unsigned int> open;
        unordered_set<unsigned int> closed;
        unsigned int start = 0;
        unsigned int goal = grid.size() - 1;
        open.push_back(0);
        closed.insert(start);
        while(!open.empty()) {
                unsigned int current = open.back(); open.pop_back();

                vector<unsigned int> children = expand(current, width, height);
                for(unsigned int i = 0; i < children.size(); i++) {
                        if(children[i] == goal) {
                                return true;
                        }
                        if(grid[children[i]] >= 0) {
                                if(closed.find(children[i]) == closed.end()) {
                                        closed.insert(children[i]);
                                        open.push_back(children[i]);
                                }
                        }
                }
        }
        return false;
}


int main(int argc, char* argv[]) {

        // Setup argtable parameters.
        struct arg_lit *help               = arg_lit0(NULL, "help","Print help.");
        struct arg_str *filename           = arg_str0(NULL, "output_filename", NULL, "Filename of the outputfile. Default: grid.dimacs");
        struct arg_int *user_seed          = arg_int0(NULL, "seed", NULL, "Seed to use for random number generator.");
        struct arg_int *pwidth             = arg_int0(NULL, "width", NULL, "Width of the maze. Default: 10");
        struct arg_int *pheight            = arg_int0(NULL, "height", NULL, "Height of the maze. Default: 10");
        struct arg_dbl *pblocked           = arg_dbl0(NULL, "blocked", NULL, "Percentage of cells in the maze to be blocked. Default: 0.3 (i.e. 30%)");
        struct arg_lit *disable_print_maze = arg_lit0(NULL, "print_path","Printing the solution at the end of the program.");
        struct arg_end *end                = arg_end(100);

        // Define argtable.
        void* argtable[] = {
                help, filename, user_seed, pwidth, pheight, pblocked, disable_print_maze,  
                end
        };
        // Parse arguments.
        int nerrors = arg_parse(argc, argv, argtable);
        const char *progname = argv[0];

        // Catch case that help was requested.
        if (help->count > 0) {

                printf("Usage: %s", progname);
                arg_print_syntax(stdout, argtable, "\n");
                arg_print_glossary(stdout, argtable,"  %-40s %s\n");
                printf("This is the experimental generate maze program.\n");
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1;
        }

        if (nerrors > 0) {
                arg_print_errors(stderr, end, progname);
                printf("Try '%s --help' for more information.\n",progname);
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1; 
        }

        string outputfilename("grid.dimacs");
        int width      = 10;
        int height     = 10;
        double blocked = 0.3;

        if(filename->count > 0) {
                outputfilename = filename->sval[0];
        }

        if( pwidth->count ) {
                width = pwidth->ival[0];
        }

        if( pheight->count ) {
                height = pheight->ival[0];
        }

        if( pblocked->count ) {
                blocked = pblocked->dval[0];
        }

        if( user_seed->count ) {
                srand( user_seed->ival[0] );
        } else {
                srand( time(NULL));
        }


        unsigned int totalCells = width * height;
        int numBlocked = ceil(blocked * totalCells);

        int attempts = 0;
        vector<int> grid(totalCells);
        do {
                for( unsigned int i = 0; i < grid.size(); i++) {
                        grid[i] = 0;
                }
                grid[0] = 1;
                grid[totalCells-1] = 2;

                for(int j = 0; j < numBlocked; j++) {
                        int cell = rand() % totalCells;
                        if(grid[cell] != 0) {
                                j--;
                        }
                        else {
                                grid[cell] = -1;
                        }
                }

                if(attempts > 10000) {
                        fprintf(stderr, "Tried 1000 times to construct instance, giving up\n");
                        return 0;
                } 
                attempts++;
        } while( !isSolvableFourway(grid, width, height) );

        if(!disable_print_maze->count ) {
                dumpInstance(stdout, grid, width, height);
        }
        std::cout <<  "writing graph to disc"  << std::endl;

        vector<vector< int > > grid2D(height);
        for( int i = 0; i < height; i++) {
                grid2D[i].resize(width);
                for( int j = 0; j < width; j++) {
                        grid2D[i][j] = grid[i*width+j];
                }
        }
        graph_io_lp io;
        io.writeGridToDIMACS( grid2D, width, height, outputfilename );

        return 0;
}
