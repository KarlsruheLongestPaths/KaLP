/******************************************************************************
 * printing.h
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


#ifndef PRINTING_AHTV5ZZ1
#define PRINTING_AHTV5ZZ1

class printing {
public:
        printing() {} ;
        virtual ~printing();

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
        }
};


#endif /* end of include guard: PRINTING_AHTV5ZZ1 */
