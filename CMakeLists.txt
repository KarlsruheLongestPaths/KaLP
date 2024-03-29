
cmake_minimum_required(VERSION 3.10)
project(KaLP VERSION 1.0 DESCRIPTION "Karlsruhe Longest Paths")

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -funroll-loops -fno-stack-limit -DNDEBUG -O3")

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}/lib/tools
    ${PROJECT_SOURCE_DIR}/lib/partitioning
    ${PROJECT_SOURCE_DIR}/lib/data_structure
    ${PROJECT_SOURCE_DIR}/extern/KaHIP/lib
    ${PROJECT_SOURCE_DIR}/extern/argtable-2.10/include
 ${PROJECT_SOURCE_DIR}/lib/LPDP
 ${PROJECT_SOURCE_DIR}/extern/KaHIP/app
 ${PROJECT_SOURCE_DIR}/extern/KaHIP/lib
 ${PROJECT_SOURCE_DIR}/extern/KaHIP/lib/tools
 ${PROJECT_SOURCE_DIR}/extern/KaHIP/lib/partition
 ${PROJECT_SOURCE_DIR}/extern/KaHIP/lib/io
 ${PROJECT_SOURCE_DIR}/extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/flow_refinement/
    /usr/include/openmpi/
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/argtable3-3.2.2)

# Add source files
set(LIBKALP_SOURCES
    lib/partitioning/partitioning.cpp
    lib/data_structure/undirected_graph.cpp
    lib/io/graph_io.cpp
    lib/LPDP/QueryManager.cpp
    lib/LPDP/Block.cpp
    lib/LPDP/BaseGraphSearch.cpp
    lib/LPDP/BlockGraphSearch.cpp
    extern/argtable3-3.2.2/argtable3.c
    # Add more source files here
)

set(LIBKAFFPA_SOURCES
extern/KaHIP/lib/data_structure/graph_hierarchy.cpp
                      extern/KaHIP/lib/algorithms/strongly_connected_components.cpp
                      extern/KaHIP/lib/algorithms/topological_sort.cpp
                      extern/KaHIP/lib/algorithms/push_relabel.cpp
                      extern/KaHIP/lib/io/graph_io.cpp
                      extern/KaHIP/lib/tools/quality_metrics.cpp
                      extern/KaHIP/lib/tools/random_functions.cpp
                      extern/KaHIP/lib/tools/graph_extractor.cpp
                      extern/KaHIP/lib/tools/misc.cpp
                      extern/KaHIP/lib/tools/partition_snapshooter.cpp
                      extern/KaHIP/lib/partition/graph_partitioner.cpp
                      extern/KaHIP/lib/partition/w_cycles/wcycle_partitioner.cpp
                      extern/KaHIP/lib/partition/coarsening/coarsening.cpp
                      extern/KaHIP/lib/partition/coarsening/contraction.cpp
                      extern/KaHIP/lib/partition/coarsening/edge_rating/edge_ratings.cpp
                      extern/KaHIP/lib/partition/coarsening/matching/matching.cpp
                      extern/KaHIP/lib/partition/coarsening/matching/random_matching.cpp
                      extern/KaHIP/lib/partition/coarsening/matching/gpa/path.cpp
                      extern/KaHIP/lib/partition/coarsening/matching/gpa/gpa_matching.cpp
                      extern/KaHIP/lib/partition/coarsening/matching/gpa/path_set.cpp
                      extern/KaHIP/lib/partition/coarsening/clustering/node_ordering.cpp
                      extern/KaHIP/lib/partition/coarsening/clustering/size_constraint_label_propagation.cpp
                      extern/KaHIP/lib/partition/initial_partitioning/initial_partitioning.cpp
                      extern/KaHIP/lib/partition/initial_partitioning/initial_partitioner.cpp
                      extern/KaHIP/lib/partition/initial_partitioning/initial_partition_bipartition.cpp
                      extern/KaHIP/lib/partition/initial_partitioning/initial_refinement/initial_refinement.cpp
                      extern/KaHIP/lib/partition/initial_partitioning/bipartition.cpp
                      extern/KaHIP/lib/partition/initial_partitioning/initial_node_separator.cpp
                      extern/KaHIP/lib/partition/uncoarsening/uncoarsening.cpp
                      extern/KaHIP/lib/partition/uncoarsening/separator/area_bfs.cpp
                      extern/KaHIP/lib/partition/uncoarsening/separator/vertex_separator_algorithm.cpp
                      extern/KaHIP/lib/partition/uncoarsening/separator/vertex_separator_flow_solver.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/cycle_improvements/greedy_neg_cycle.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/cycle_improvements/problem_factory.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/cycle_improvements/augmented_Qgraph.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/mixed_refinement.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/label_propagation_refinement/label_propagation_refinement.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/refinement.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/2way_fm_refinement/two_way_fm.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/flow_refinement/two_way_flow_refinement.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/flow_refinement/boundary_bfs.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/flow_refinement/flow_solving_kernel/cut_flow_problem_solver.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/flow_refinement/most_balanced_minimum_cuts/most_balanced_minimum_cuts.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/quotient_graph_refinement.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/complete_boundary.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/partial_boundary.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/quotient_graph_scheduling/quotient_graph_scheduling.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/quotient_graph_scheduling/simple_quotient_graph_scheduler.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/quotient_graph_scheduling/active_block_quotient_graph_scheduler.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/kway_graph_refinement/kway_graph_refinement.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/kway_graph_refinement/kway_graph_refinement_core.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/kway_graph_refinement/kway_graph_refinement_commons.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/cycle_improvements/augmented_Qgraph_fabric.cpp 
                      extern/KaHIP/lib/partition/uncoarsening/refinement/cycle_improvements/advanced_models.cpp 
                      extern/KaHIP/lib/partition/uncoarsening/refinement/kway_graph_refinement/multitry_kway_fm.cpp 
                      extern/KaHIP/lib/partition/uncoarsening/refinement/node_separators/greedy_ns_local_search.cpp 
                      extern/KaHIP/lib/partition/uncoarsening/refinement/node_separators/fm_ns_local_search.cpp 
                      extern/KaHIP/lib/partition/uncoarsening/refinement/node_separators/localized_fm_ns_local_search.cpp 
                      extern/KaHIP/lib/algorithms/cycle_search.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/cycle_improvements/cycle_refinement.cpp
                      extern/KaHIP/lib/parallel_mh/galinier_combine/gal_combine.cpp
                      extern/KaHIP/lib/parallel_mh/galinier_combine/construct_partition.cpp
                      extern/KaHIP/lib/partition/uncoarsening/refinement/tabu_search/tabu_search.cpp    
)

set(KALP_SOURCES
    app/kalp.cpp
)

set(RANDOM_SUBGRAPH_SOURCES
    app/random_subgraph.cpp
)

set(GENERATE_MAZE_SOURCES
    app/generate_maze.cpp
)

# Executable targets
add_executable(kalp ${KALP_SOURCES} ${LIBKALP_SOURCES} ${LIBKAFFPA_SOURCES})
add_executable(random_subgraph ${RANDOM_SUBGRAPH_SOURCES} ${LIBKALP_SOURCES} ${LIBKAFFPA_SOURCES})
add_executable(generate_maze ${GENERATE_MAZE_SOURCES} ${LIBKALP_SOURCES} ${LIBKAFFPA_SOURCES})

# Find required libraries
find_package(OpenMP REQUIRED)

# Link libraries
target_link_libraries(kalp PUBLIC OpenMP::OpenMP_CXX tbb gomp)
target_link_libraries(random_subgraph PUBLIC OpenMP::OpenMP_CXX tbb gomp)
target_link_libraries(generate_maze PUBLIC OpenMP::OpenMP_CXX tbb gomp)

