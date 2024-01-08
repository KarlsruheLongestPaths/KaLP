/******************************************************************************
 * constants.hpp
 *
 * Source of KaLP -- Karlsruhe Longest Paths 
 ******************************************************************************
 * Copyright (C) 2018 Kai Fieger
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

#ifndef LP_CONSTANTS_HPP
#define LP_CONSTANTS_HPP

#include <iostream>
#include <omp.h>
#include <limits>

using Index = unsigned;
using Level = unsigned;
using NodeID = unsigned;
using BlockID = unsigned;
using Weight = int;

using STEP_COUNT = unsigned long long;

constexpr Index invalid_index = std::numeric_limits<Index>::max();
constexpr Level invalid_level = std::numeric_limits<Level>::max();
constexpr NodeID invalid_node = std::numeric_limits<NodeID>::max();
constexpr BlockID invalid_block = std::numeric_limits<BlockID>::max();//4294967295u;
constexpr Weight invalid_weight = std::numeric_limits<Weight>::max()-1;
constexpr Weight infinite_weight = std::numeric_limits<Weight>::max();
//don't change weight to some unsigned type
constexpr Weight neg_infinite_weight = std::numeric_limits<Weight>::min();

inline void ensure(bool condition, const char *message) {
#ifndef NDEBUG
    if (!condition) {
        throw std::invalid_argument(message);
    }
#else
    (void)condition;
    (void)message;
#endif
}

#endif //LP_CONSTANTS_HPP
