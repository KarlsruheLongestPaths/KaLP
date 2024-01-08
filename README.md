Karlsruhe Longest Paths (KaLP)
=====

The KaLP framework -- Karlsruhe Longest Paths.

The longest path problem (LP) is to find a simple path of maximum length between two given vertices of a graph where length is defined as the number of edges or the total weight of the edges in the path.  The problem is known to be NP-complete and has several applications such as designing circuit boards, project planning, information retrieval or patrolling algorithms for multiple robots in graphs. 

We proposed an optimal algorithm for solving the longest path problem in undirected graphs [1]. By using graph partitioning and dynamic programming, we designed an algorithm,
which is significantly faster than other state-of-the-art methods and can solve more instances. Here we provide the implementation of the algorithm as easy to use open source software.


[1] https://arxiv.org/pdf/1702.04170.pdf

[2] Main project site: http://algo2.iti.kit.edu/kalp

Installation Notes
=====

Before you can start you need to install the following software packages:

- Scons (http://www.scons.org/)
- Argtable (http://argtable.sourceforge.net/)

Once you installed the packages, just type ./compile.sh.
Once you did that without errors you can try to run the following command:

./deploy/kalp examples/Grid8x8.graph --start_vertex=0 --target_vertex=63

For a description of the graph format please have look into the manual.
