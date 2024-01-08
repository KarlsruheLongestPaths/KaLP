/******************************************************************************
 * MatchingAndPath.hpp
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
 
#ifndef LP_MATCHINGANDPATH_HPP
#define LP_MATCHINGANDPATH_HPP

#include "constants.hpp"
#include <vector>
//renamed from "Matching" to "BNodePairs" to avoid conflict with the "Matching" of KaHIP 
class BNodePairs {
private:
    std::vector<NodeID> matching;
public:
    BNodePairs(unsigned number_of_boundary_nodes = 0) : matching(number_of_boundary_nodes, invalid_node) {}
//TODO [] and size not used?
    NodeID& operator[](unsigned i) {
        return matching[i];
    }

    unsigned size() const {
        return matching.size();
    }

    const std::vector<NodeID>& get_vector() const {
        return matching;
    }

    void insert(NodeID a) {
        ensure(matching[a] == invalid_node, "a already used");
        matching[a] = a;
    }

    void remove(NodeID a) {
        ensure(matching[a] == a, "does not contain a");
        matching[a] = invalid_node;
    }

    void insert(NodeID a, NodeID b) {
        ensure(a != b, "trying to insert pair {a,a}");
        ensure(matching[a] == invalid_node && matching[b] == invalid_node, "a or b already used");
        matching[a] = b;
        matching[b] = a;
    }

    void remove(NodeID a, NodeID b) {
        ensure(a != b, "trying to remove pair {a,a}");
        ensure(matching[a] == b && matching[b] == a, "does not contain a or b");
        matching[a] = invalid_node;
        matching[b] = invalid_node;
    }
};

namespace std {
    template<> struct hash<BNodePairs> {
        std::size_t operator()(const BNodePairs& matching) const {
            std::size_t hash = 0;
            for (auto node : matching.get_vector()) {
                hash ^= node + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };

    template<> struct equal_to<BNodePairs> {
        bool operator()(const BNodePairs& x, const BNodePairs& y) const {
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
}

class Path {
private:
    std::vector<BNodePairs> matchings; //per subblock
public:
    Weight weight;
    std::vector<NodeID> edges; //boundary edges or path, depending on useage


    Path() : weight{0}, matchings(0) {}

    Path(std::vector<unsigned> number_of_nodes_per_subblock) : weight{0}, matchings(std::vector<BNodePairs>(number_of_nodes_per_subblock.size())) {
        for (BlockID block_id = 0; block_id < matchings.size(); block_id++) {
            matchings[block_id] = BNodePairs(number_of_nodes_per_subblock[block_id]);
        }
    }

    BNodePairs& operator[](unsigned i) {
        return matchings[i];
    }

    void reserve(unsigned nodes) {
        edges.reserve(nodes);
    }

    unsigned size() const {
        return matchings.size();
    }
};

#endif //LP_MATCHINGANDPATH_HPP
