#!/bin/bash
source ./extern/tbb2019_20181203oss/bin/tbbvars.sh intel64 linux auto_tbbroot

NCORES=4
unamestr=`uname`
if [[ "$unamestr" == "Linux" ]]; then
        NCORES=`grep -c ^processor /proc/cpuinfo`
fi
#darwin should not work anymore vvv (because of tbb)
if [[ "$unamestr" == "Darwin" ]]; then
        NCORES=`sysctl -n hw.ncpu`
fi

mkdir bin 
cd bin 
cmake ../
make -j $NCORES
cd ..

rm -rf deploy

cd extern/KaHIP3.16
./compile_withcmake.sh
cd ..
cd ..


mkdir deploy
cp ./bin/kalp deploy/
cp ./bin/generate_maze deploy/
cp ./bin/random_subgraph deploy/
cp ./extern/KaHIP3.16/deploy/graphchecker deploy/

