Karlsruhe Longest Paths (KaLP)
=====

The KaLP framework -- Karlsruhe Longest Paths.

The longest path problem (LP) is to find a simple path of maximum length between two given vertices of a graph where length is defined as the number of edges or the total weight of the edges in the path.  The problem is known to be NP-complete and has several applications such as designing circuit boards, project planning, information retrieval or patrolling algorithms for multiple robots in graphs. 

We proposed an optimal algorithm for solving the longest path problem in undirected graphs [1, 2]. By using graph partitioning and dynamic programming, we designed an algorithm,
which is significantly faster than other state-of-the-art methods and can solve more instances. Here we provide the implementation of the algorithm as easy to use open source software.

<p align="center">
<img src="./img/lpath.png"
  alt="framework overview"
  width="601" height="558">
</p>

[1] Longest Paths Sequentially: https://arxiv.org/pdf/1702.04170.pdf

[2] Longest Paths Parallel: https://arxiv.org/pdf/1905.03645.pdf

[2] Main project site: http://karlsruhelongestpaths.github.io

Installation Notes
=====

Before you can start you need to install the following software packages:

- CMake >= v3.10 

Once you installed the packages, just type ./compile.sh.
Once you did that without errors you can try to run the following command:

./deploy/kalp examples/Grid8x8.graph --start_vertex=0 --target_vertex=63

For a description of the graph format please have look into the manual.

License
=====
The program is licenced under GPL 3.0. Please let us know if you need a commercial licence. (We also give away MIT licence on request).
If you publish results using our algorithms, please acknowledge our work by quoting the following paper (PDF):

````
@inproceedings{DBLP:conf/socs/FiegerB0S19,
  author       = {Kai Fieger and
                  Tom{\'{a}}s Balyo and
                  Christian Schulz and
                  Dominik Schreiber},
  title        = {Finding Optimal Longest Paths by Dynamic Programming in Parallel},
  booktitle    = {{SOCS}},
  pages        = {61--69},
  publisher    = {{AAAI} Press},
  year         = {2019}
}
````
