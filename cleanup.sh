#!/bin/bash
source ./extern/tbb2019_20181203oss/bin/tbbvars.sh intel64 linux auto_tbbroot

scons program=kalp variant=optimized -j 4 -c 
scons program=random_subgraph variant=optimized -j 4 -c 
scons program=generate_maze variant=optimized -j 4 -c 

rm -rf deploy
rm -rf optimized
rm config.log
