#!/bin/bash

echo "Configuring..."
echo "-----------------------------------------------------------------------"
./scripts/config-build.py -c intel@16.0.109 --buildtype RelWithDebInfo -DENABLE_CXX11=FALSE
if [ $? -ne 0 ]; then
    echo "Error: config-build.py failed"
    exit 1
fi
echo "-----------------------------------------------------------------------"

cd build-chaos-intel@16.0.109-relwithdebinfo
    echo "Generating C/Fortran binding..."
    make VERBOSE=1 generate
    if [ $? -ne 0 ]; then
        echo "Error: 'make test' failed"
        exit 1
    fi
    echo "-----------------------------------------------------------------------"

    echo "Building..."
    echo "-----------------------------------------------------------------------"
    make VERBOSE=1 -j16
    if [ $? -ne 0 ]; then
        echo "Error: 'make' failed"
        exit 1
    fi
    echo "-----------------------------------------------------------------------"

    echo "Run tests"
    echo "-----------------------------------------------------------------------"
    make test ARGS="-T Test"
    if [ $? -ne 0 ]; then
        echo "Error: 'make test' failed"
        exit 1
    fi
    echo "-----------------------------------------------------------------------"
cd ..
