#!/usr/bin/bash

BASH_BUILD_TYPE_RELEASE="release"
BASH_BUILD_TYPE_DEBUG="debug"
BASH_BUILD_TYPE=$BASH_BUILD_TYPE_RELEASE
THIS_DIR=`pwd`
BASH_INSTALL=false
BASH_PACKAGE=false
BASH_PREFIX=""

while getopts "t:-:" opt; do
  case $opt in
    -)
      case "${OPTARG}" in
        install)
          BASH_INSTALL=true
          ;;
        package)
          BASH_PACKAGE=true
          ;;
        prefix)
          val="${!OPTIND}"; OPTIND=$(( $OPTIND + 1 ))
          BASH_PREFIX=$val
      esac;;
    t)
      if [ "${OPTARG,,}" == $BASH_BUILD_TYPE_RELEASE ]; then
        BASH_BUILD_TYPE=$BASH_BUILD_TYPE_RELEASE
      elif [ "${OPTARG,,}" == $BASH_BUILD_TYPE_DEBUG ]; then
        BASH_BUILD_TYPE=$BASH_BUILD_TYPE_DEBUG
      else
        echo "Invalid build type " $OPTARG
        exit -1
      fi
      ;;
  esac
done


cd build
cmake -DCMAKE_INSTALL_PREFIX=$BASH_PREFIX -DCMAKE_BUILD_TYPE=$BASH_BUILD_TYPE ..
make -j8
if [[ $BASH_INSTALL == true ]]; then
  make install
fi

if [[ $BASH_PACKAGE == true ]]; then
  make package
fi
