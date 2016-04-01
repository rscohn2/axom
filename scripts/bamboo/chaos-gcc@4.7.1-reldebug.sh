#!/bin/sh
echo "Configuring..."
echo "-----------------------------------------------------------------------"
./scripts/config-build.py -c gcc@4.7.1 --buildtype RelWithDebInfo -DENABLE_CXX11=FALSE
cd build-chaos-gcc@4.7.1-relwithdebinfo
echo "-----------------------------------------------------------------------"

echo "-----------------------------------------------------------------------"
echo "Generating C/Fortran binding..."
make generate
echo "-----------------------------------------------------------------------"

echo "Building..."
echo "-----------------------------------------------------------------------"
make -j16
echo "-----------------------------------------------------------------------"

echo "Run tests"
echo "-----------------------------------------------------------------------"
make test ARGS="-T Test"
echo "-----------------------------------------------------------------------"
