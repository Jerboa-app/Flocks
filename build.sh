#!/bin/bash
WINDOWS=1
while [[ $# -gt 0 ]]; do
  case $1 in
    -w|--windows)
      WINDOWS=0
      shift # past argument
      shift # past value
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done


for file in CMakeFiles cmake_install.cmake CMakeCache.txt Makefile Jerboa
do
  if [ -d $file ];
  then
    rm -rf $file
  fi
  if [ -f $file ];
  then
    rm $file
  fi
done

echo $WINDOWS
if [[ $WINDOWS -eq 0 ]];
then 
  cmake . -D WINDOWS=ON -D CMAKE_TOOLCHAIN_FILE=./windows.cmake && make
else
  cmake . && make
fi
