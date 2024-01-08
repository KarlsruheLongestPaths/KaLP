/******************************************************************************
 * graph_io_lp.cpp 
 *
 * Source of KaLP -- Karlsruhe Longest Paths.
 *
 ******************************************************************************
 * Copyright (C) 2017 Christian Schulz <christian.schulz@kit.edu>
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

#include <sstream>
#include "graph_io.h"
#include "tools/helpers.h"


graph_io_lp::graph_io_lp() {

}

graph_io_lp::~graph_io_lp() {

}


int graph_io_lp::readGraph(UndirectedGraph & G, std::string filename) {

        std::string dimacs_ending(".dimacs");
        std::string gr_ending(".gr");
        std::string graph_ending(".graph");

        if( !hasEnding(filename, dimacs_ending) && !hasEnding(filename, gr_ending) && !hasEnding(filename, graph_ending) ) {
                std::cout <<  "file ending .gr or .dimacs or .graph needed to use the correct graph reader."  << std::endl;
                exit(0);
        }
        if( hasEnding(filename, dimacs_ending) || hasEnding(filename, gr_ending) || !hasEnding(filename, graph_ending) ) {
                return readDIMACS(G, filename);
        }

        std::string line;
        // open file for reading
        std::ifstream in(filename.c_str());
        if (!in) {
                std::cerr << "Error opening " << filename << std::endl;
                return 1;
        }

        long nmbNodes;
        long nmbEdges;

        std::getline(in,line);
        //skip comments
        while( line[0] == '%' ) {
                std::getline(in, line);
        }

        int ew = 0;
        std::stringstream ss(line);
        ss >> nmbNodes;
        ss >> nmbEdges;
        ss >> ew;

        if( 2*nmbEdges > std::numeric_limits<int>::max() || nmbNodes > std::numeric_limits<int>::max()) {
                std::cerr <<  "The graph is too large. Currently only 32bit supported!"  << std::endl;
                exit(0);
        }

        bool read_ew = false;
        bool read_nw = false;

        if(ew == 1) {
                read_ew = true;
        } else if (ew == 11) {
                read_ew = true;
                read_nw = true;
        } else if (ew == 10) {
                read_nw = true;
        }
        nmbEdges *= 2; //since we have forward and backward edges

        NodeID node_counter   = 0;
        EdgeID edge_counter   = 0;

        while(  std::getline(in, line)) {

                if (line[0] == '%') { // a comment in the file
                        continue;
                }

                NodeID node = G.addNode(); node_counter++;

                std::stringstream ss(line);

                NodeWeight weight = 1;
                if( read_nw ) {
                        ss >> weight; // do nothing with the node weight
                }

                NodeID target;
                while( ss >> target ) {
                        //check for self-loops
                        if(target-1 == node) {
                                std::cerr <<  "The graph file contains self-loops. This is not supported. Please remove them from the file."  << std::endl;
                        }

                        EdgeWeight edge_weight = 1;
                        if( read_ew ) {
                                ss >> edge_weight;
                        }
                        edge_counter++;
                        G.addEdgeDirected(node, target-1, edge_weight);
                }

                if(in.eof()) {
                        break;
                }
        }

        if( edge_counter != (EdgeID) nmbEdges ) {
                std::cerr <<  "number of specified edges mismatch"  << std::endl;
                std::cerr <<  edge_counter <<  " " <<  nmbEdges  << std::endl;
                exit(0);
        }

        if( node_counter != (NodeID) nmbNodes) {
                std::cerr <<  "number of specified nodes mismatch"  << std::endl;
                std::cerr <<  node_counter <<  " " <<  nmbNodes  << std::endl;
                exit(0);
        }
        std::cout <<  "done reading graph"  << std::endl;

        return 0;
}

int graph_io_lp::readDIMACS(UndirectedGraph & G, std::string filename) {
        //TODO: this is currently only rudimentary implemented, 
        //TODO: error handling and outputing nice information to the user
        std::string line;

        // open file for reading
        std::ifstream in(filename.c_str());
        if (!in) {
                std::cerr << "Error opening " << filename << std::endl;
                return 1;
        } else {
                std::cout <<  "Reading DIMACS file. Attention: graph has to be bidirected and weights have to be symmetric."  << std::endl;
        }

        NodeID nmbNodes;
        NodeID nmbEdges;
        std::getline(in,line);
        char first_char;
        while( !in.eof() ) {
                std::stringstream ss(line);
                ss >> first_char;

                switch( first_char ) {
                        case 'c':
                                break;
                        case '\n':
                                break;
                        case '\0':
                                break;
                        case 'p': 
                                {
                                        std::string type;
                                        ss >> type;
                                        ss >> nmbNodes;
                                        ss >> nmbEdges;
                                        for( unsigned i = 0; i < nmbNodes; i++) {
                                                G.addNode();
                                        }
                                }

                                break;
                        case 'n': 
                                {
                                        NodeID node; char ntype;
                                        ss >> node;
                                        ss >> ntype;
                                }
                                break;
                        case 'a': 
                                {
                                        NodeID source;
                                        NodeID target;
                                        int weight;
                                        ss >> source;
                                        ss >> target;
                                        ss >> weight;
                                        G.addEdgeDirected(source-1, target-1, weight);
                                }
                                break;

                }

                std::getline(in, line);
        }
        std::cout <<  "Done reading graph"  << std::endl;
        return 0;
}


int graph_io_lp::writeGridToDIMACS(std::vector< std::vector< int > > grid2D, int width, int height, std::string filename) {
        std::ofstream f(filename.c_str());
        // check how many edges there are
        int edges = 0;

        for( int i = 0; i < height; i++) {
                for( int j = 0; j < width; j++) {
                        if( grid2D[i][j] ==  -1 ) continue;
                        if( i-1 >= 0 && grid2D[i-1][j] != -1) {
                                edges++;
                        }
                        if( i+1 < height && grid2D[i+1][j] != -1) {
                                edges++;
                        }
                        if( j-1 >= 0 && grid2D[i][j-1] != -1) {
                                edges++;
                        }
                        if( j+1 < width && grid2D[i][j+1] != -1) {
                                edges++;
                        }
                }
        }
        f << "p sp " << width*height << " " << edges << std::endl;
        for( int i = 0; i < height; i++) {
                for( int j = 0; j < width; j++) {
                        if( grid2D[i][j] ==  -1 ) continue;
                        if( i-1 >= 0 && grid2D[i-1][j] != -1) {
                                f << "a " << i*height+j+1 << " " << (i-1)*height+j+1 << " " << 1 << std::endl;
                        }
                        if( i+1 < height && grid2D[i+1][j] != -1) {
                                f << "a " << i*height+j+1 << " " << (i+1)*height+j+1 << " " << 1 << std::endl;
                        }
                        if( j-1 >= 0 && grid2D[i][j-1] != -1) {
                                f << "a " << i*height+j+1 << " " << (i)*height+j << " " << 1 << std::endl;
                        }
                        if( j+1 < width && grid2D[i][j+1] != -1) {
                                f << "a " << i*height+j+1 << " " << (i)*height+j+2 << " " << 1 << std::endl;
                        }
                }
        }
        f.close();
        return 0;

}
