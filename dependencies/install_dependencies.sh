#!/bin/bash
source versions.sh

CLEAN_UP=0
for arg in "$@"
do
    case $arg in
        -c|--cleanup)
        CLEAN_UP=1
        shift # Remove
        ;;
        *)
        shift # Remove generic argument from processing
        ;;
    esac
done

# libraries
HELIB_GIT=https://github.com/homenc/HElib.git




git clone ${HELIB_GIT}

# start building
# build HElib
echo "Building HElib"
cd HElib
git checkout ${HELIB_COMMIT}
rm -rf build
mkdir build
cd build
cmake -DPACKAGE_BUILD=ON -DCMAKE_INSTALL_PREFIX="${DEPENDENCIES_DIR}"/install/ ..
make -j16
make install
cp ${HELIB_DIR}/lib/libhelib.a ${HELIB_DIR}/lib/fhe.a
cd "${WD}"
# clean up
if [ $CLEAN_UP -eq 1 ]; then
    rm -rf HElib
fi




SEAL_GIT=https://github.com/homenc/HElib.git

git clone ${SEAL_GIT}

# start building
# build SEAL
echo "Building SEAL"
cd SEAL
# git checkout ${HELIB_COMMIT}
rm -rf build
# Note: Microsoft SEAL compiled with Clang++ has much better runtime performance than one compiled with GNU G++.
CC=/usr/bin/clang CXX=/usr/bin/clang++ cmake -S . -B build -DCMAKE_INSTALL_PREFIX="${DEPENDENCIES_DIR}"/install/SEAL -DSEAL_USE_INTEL_HEXL=ON -DSEAL_USE_MSGSL=ON
CC=/usr/bin/clang CXX=/usr/bin/clang++ cmake --build build
sudo cmake --install build
cd "${WD}"
# clean up
if [ $CLEAN_UP -eq 1 ]; then
    rm -rf SEAL
fi