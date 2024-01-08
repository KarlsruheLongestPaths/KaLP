/******************************************************************************
 * graph_io_lp.h 
 *
 * Source of KaLP -- Karlsruhe Longest Paths.
 *
 ******************************************************************************
 * Copyright (C) 2013-2015 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
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

#ifndef GRAPHIO_H_LP
#define GRAPHIO_H_LP

#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "definitions.h"
#include "undirected_graph.h"

class graph_io_lp {
        public:
                graph_io_lp();
                virtual ~graph_io_lp () ;

                static 
                int readGraph(UndirectedGraph & G, std::string filename);

                static
                int readDIMACS(UndirectedGraph & G, std::string filename);

                static
                int writeGridToDIMACS(std::vector< std::vector< int > > grid2D, int width, int height, std::string filename);

                template<typename vectortype> 
                static void writeVector(std::vector<vectortype> & vec, std::string filename);

                template<typename vectortype> 
                static void readVector(std::vector<vectortype> & vec, std::string filename);


};

template<typename vectortype> 
void graph_io_lp::writeVector(std::vector<vectortype> & vec, std::string filename) {
        std::ofstream f(filename.c_str());
        for( unsigned i = 0; i < vec.size(); ++i) {
                f << vec[i] <<  std::endl;
        }

        f.close();
}

template<typename vectortype> 
void graph_io_lp::readVector(std::vector<vectortype> & vec, std::string filename) {

        std::string line;

        // open file for reading
        std::ifstream in(filename.c_str());
        if (!in) {
                std::cerr << "Error opening vectorfile" << filename << std::endl;
                return;
        }

        unsigned pos = 0;
        std::getline(in, line);
        while( !in.eof() ) {
                if (line[0] == '%') { //Comment
                        continue;
                }

                vectortype value = (vectortype) atof(line.c_str());
                vec[pos++] = value;
                std::getline(in, line);
        }

        in.close();
}

#endif /*GRAPHIO_H_*/
