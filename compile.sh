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

rm -rf deploy
for program in kalp generate_maze random_subgraph; do 
scons program=$program variant=optimized -j $NCORES
if [ "$?" -ne "0" ]; then 
        echo "compile error in $program. exiting."
        exit
fi
done

cd extern/KaHIP
scons program=graphchecker variant=optimized -j 4
if [ "$?" -ne "0" ]; then 
        echo "compile error in graph checker! we continue."
        exit
fi
cd ..
cd ..

mkdir deploy
cp ./optimized/kalp deploy/
cp ./optimized/generate_maze deploy/
cp ./optimized/random_subgraph deploy/
cp ./extern/KaHIP/optimized/graphchecker deploy/
rm -rf optimized
rm config.log

