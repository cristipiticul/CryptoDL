#!/bin/sh
CMAKE_VERSION=3.10.2
GMP_VERSION=6.2.0
NTL_VERSION=11.4.3
HELIB_COMMIT=286e4bc0f585dc9ace754fd755ad09d91a4648e7


# Absolute path to this script, e.g. /home/user/bin/foo.sh
SCRIPT=$(readlink -f "$0")
DEPENDENCIES_DIR=$(dirname "$SCRIPT")
# this works only in bash
# DEPENDENCIES_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
#directories
# install dirs
HELIB_DIR="${DEPENDENCIES_DIR}"/install/helib_pack
SEAL_DIR="${DEPENDENCIES_DIR}"/install/SEAL


# include dirs 
HELIB_INCLUDE_DIR="${HELIB_DIR}"/include 
SEAL_INCLUDE_DIR="${SEAL_DIR}"/include/SEAL-3.7

# generate a makefile that can be included
MAKEFILE="${DEPENDENCIES_DIR}"/makefile.versions
echo "# this file is auto generated. edit version.sh instead" > "${MAKEFILE}"
echo "DEP_INCLUDES := -I${HELIB_INCLUDE_DIR} -I${SEAL_INCLUDE_DIR}" >> "${MAKEFILE}"
# echo "DEP_LIBS := ${HELIB_DIR}/fhe.a -L${NTL_DIR}/lib -lntl -L${GMP_DIR}/lib -lgmp -lboost_filesystem -lpthread -lboost_system -ljpeg" >> "${MAKEFILE}"
echo "DEP_LIBS := ${HELIB_DIR}/lib/fhe.a -L${HELIB_DIR}/lib ${SEAL_DIR}/lib/libseal-3.7.a -L${SEAL_DIR}/lib -lntl -lboost_filesystem -lpthread -lboost_system -ljpeg" >> "${MAKEFILE}"
echo "DEP_RPATH := -Wl,-rpath=${HELIB_DIR}/lib/,-rpath=${SEAL_DIR}" >> "${MAKEFILE}"
