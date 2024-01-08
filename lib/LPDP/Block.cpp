/******************************************************************************
 * Block.cpp
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
 
#include "Block.hpp"
#include "BaseGraphSearch.hpp"
#include "BlockGraphSearch.hpp"

/*
 * IndexRange+Iterator
 */

IndexRange::IndexRange(unsigned begin_pos, unsigned end_pos) : begin_pos{begin_pos}, end_pos{end_pos} {}
IndexIterator IndexRange::begin() const {
    return IndexIterator(begin_pos);
}
IndexIterator IndexRange::end() const {
    return IndexIterator(end_pos);
}
unsigned IndexRange::size() const {
    return end_pos - begin_pos;
}


IndexIterator::IndexIterator(unsigned pos) :  pos{pos} {}
IndexIterator& IndexIterator::operator++() {
    pos++;
    return *this;
}
const unsigned IndexIterator::operator*() {
    return pos;
}
bool IndexIterator::operator==(IndexIterator& other) const {
    return pos == other.pos;
}
bool IndexIterator::operator!=(IndexIterator& other) const {
    return pos != other.pos;
}

/*
 * BoundaryEdgeRange+Iterator
 */


BoundaryEdgeRange::BoundaryEdgeRange(const std::vector<NodeID> &target_node, const std::vector<BlockID> &target_block, const std::vector<Weight> &weight, unsigned begin_pos, unsigned end_pos)
        : target_node{&target_node}, target_block{&target_block}, weight{&weight}, begin_pos{begin_pos}, end_pos{end_pos} {}
BoundaryEdgeIterator BoundaryEdgeRange::begin() const {
   return BoundaryEdgeIterator(*target_node, *target_block, *weight, begin_pos);
}
BoundaryEdgeIterator BoundaryEdgeRange::end() const {
    return BoundaryEdgeIterator(*target_node, *target_block, *weight, end_pos);
}
unsigned BoundaryEdgeRange::size() const {
    return end_pos - begin_pos;
}


BoundaryEdgeIterator::BoundaryEdgeIterator(const std::vector<NodeID> &target_node, const std::vector<BlockID> &target_block, const std::vector<Weight> &weight, unsigned pos)
        : target_node{&target_node}, target_block{&target_block}, weight{&weight}, pos{pos} {}

BoundaryEdgeIterator& BoundaryEdgeIterator::operator++() {
    pos++;
    return *this;
}
const BoundaryEdge& BoundaryEdgeIterator::operator*() {
    boundary_edge.target_node = (*target_node)[pos];
    boundary_edge.target_block = (*target_block)[pos];
    boundary_edge.weight = (*weight)[pos];
    return boundary_edge;
}
bool BoundaryEdgeIterator::operator==(BoundaryEdgeIterator& other) const {
    return pos == other.pos;
}
bool BoundaryEdgeIterator::operator!=(BoundaryEdgeIterator& other) const {
    return pos != other.pos;
}


/*
 * Block
 */


IndexRange Block::boundary_nodes() {
    return IndexRange(0, number_of_boundary_nodes);
}

BoundaryEdgeRange Block::boundary_edges(NodeID node) {
    return BoundaryEdgeRange(target_node, target_block, weight, first_out[node], first_out[node + 1]);
}

STEP_COUNT Block::solve_block() {
    if (!solved) {
        solved = true;
        BlockGraphSearch search(*this);
        return search.run(1, 0, 0);
    }
    std::cout << "duplicate" << std::endl;
    return 0;
}

STEP_COUNT Block::solve_block(unsigned number_of_threads, unsigned split_steps, unsigned threshold) {
    if (!solved) {
        solved = true;
        BlockGraphSearch search(*this);
        return search.run(number_of_threads, split_steps, threshold);
    }
    std::cout << "duplicate" << std::endl;
    return 0;
}

STEP_COUNT BaseBlock::solve_block() {
    if (!solved) {
        solved = true;
        BaseGraphSearch search(*this);
        return search.run(1, 0, 0);
    }
    std::cout << "duplicate" << std::endl;
    return 0;
}

STEP_COUNT BaseBlock::solve_block(unsigned number_of_threads, unsigned split_steps, unsigned threshold) {
    if (!solved) {
        solved = true;
        BaseGraphSearch search(*this);
        return search.run(number_of_threads, split_steps, threshold);
    }
    std::cout << "duplicate" << std::endl;
    return 0;
}