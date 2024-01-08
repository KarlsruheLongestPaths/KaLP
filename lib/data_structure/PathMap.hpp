/******************************************************************************
 * PathMap.hpp
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
 
#ifndef LP_PATHMAP_HPP
#define LP_PATHMAP_HPP

#include "constants.hpp"
#include "MatchingAndPath.hpp"
#include <unordered_map>
#include <tbb/concurrent_hash_map.h>

struct matching_hash_compare {
    static size_t hash(const BNodePairs& matching) {
        std::size_t hash = 0;
        for (auto node : matching.get_vector()) {
            hash ^= node + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }

    static bool equal( const BNodePairs& x, const BNodePairs& y ) {
        const std::vector<NodeID> &vector1{x.get_vector()};
        const std::vector<NodeID> &vector2{y.get_vector()};

        ensure(vector1.size() == vector2.size(), "Matchings of different sizes are compared.");

        for (unsigned i = 0; i < vector1.size(); i++) {
            if (vector1[i] != vector2[i]) {
                return false;
            }
        }
        return true;
    }
};

class PathMap : public tbb::concurrent_hash_map<BNodePairs, Path, matching_hash_compare, tbb::tbb_allocator<std::pair<BNodePairs, Path>>> {
public:
    PathMap() {}

    void insert_or_update(const BNodePairs& key, const Path& value) {
        PathMap::accessor acc;
        if (!insert(acc, std::make_pair(key, value))) { //try to insert. false => acc locked existing item TODO hopefully
            if (value.weight > acc->second.weight) {
                acc->second = value;
            }
        }
    }

    void insert_or_update2(const BNodePairs& key, const Path& value) {
        PathMap::const_accessor read_acc;
        if (this->find(read_acc, key)) {
            if (value.weight > read_acc->second.weight) {
                read_acc.release();
                PathMap::accessor acc;
                this->find(acc, key);
                if (value.weight > acc->second.weight) {
                    acc->second = value;
                }
            }
        } else {
            PathMap::accessor acc;
            if (!insert(acc, std::make_pair(key, value))) { //try to insert. false => acc locked existing item TODO hopefully
                if (value.weight > acc->second.weight) {
                    acc->second = value;
                }
            }
        }
    }

    Weight get_weight(const BNodePairs& key) const {
        PathMap::const_accessor acc;
        this->find(acc, key);
        return acc->second.weight;
    }
};

#endif //LP_PATHMAP_HPP
