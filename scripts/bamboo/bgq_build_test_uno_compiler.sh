#!/bin/bash
# 10-20-2016 chang28, 
# 10-20-2016 chang28,  bgq_build_test_uno_compiler.sh "Debug" "" "gcc@4.7.2"
# 01-13-2017, chang28, turn off testing

echo bgq version 1.0.0
#BT="Debug"
BUILD_TYPE=$1
BUILD_PATH="atk_build"
INSTALL_PATH="atk_install"
COMP_OPT=""
BUILD_OPT=$2

BUILD=true
TEST=false
DOC=false
INSTALL_FILES=true
INSTALL_DOCS=false

JOBS=16

TOOLKIT_WEB_ROOT="/usr/global/web-pages/lc/www/toolkit"
DOCS_DIR_OLD="${TOOLKIT_WEB_ROOT}/docs_old"
DOCS_DIR="${TOOLKIT_WEB_ROOT}/docs"

COMPILER=$3
#if [[ $HOSTNAME == rz* ]]; then
#    HOST_CONFIGURATION="host-configs/rzmerl-chaos_5_x86_64_ib-${COMPILER}.cmake"
#else
#    HOST_CONFIGURATION="host-configs/surface-chaos_5_x86_64_ib-${COMPILER}.cmake"
#fi
    HOST_CONFIGURATION="host-configs/vulcanlac-bgqos_0-${COMPILER}.cmake"

OPTIONS="-ecc -hc $HOST_CONFIGURATION -bt $BUILD_TYPE -bp $BUILD_PATH -ip $INSTALL_PATH $COMP_OPT $BUILD_OPT"
echo Running $COMPILER
. ./scripts/bamboo/main_script.sh
if [ $? -ne 0 ]; then
    echo Error: calling  $COMPILER  failed
    exit 1
fi
