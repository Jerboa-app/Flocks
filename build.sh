#!/bin/bash
WINDOWS=1
OSX=1
RELEASE=0
while [[ $# -gt 0 ]]; do
  case $1 in
    -w|--windows)
      WINDOWS=0
      shift # past argument
      ;;
    -o|--osx)
      OSX=0
      shift
      ;;
    -r|--release)
      RELEASE=1
      shift
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


for file in build CMakeFiles cmake_install.cmake CMakeCache.txt Makefile Jerboa
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

echo "release ${RELEASE}"

if [[ $WINDOWS -eq 0 ]];
then 
  cmake -E make_directory build
  cmake -E chdir build cmake .. -D WINDOWS=ON -D RELEASE=$RELEASE -D CMAKE_TOOLCHAIN_FILE=./windows.cmake && make -C build
elif [[ $OSX -eq 0 ]];
then
  cmake -E make_directory build
  cmake -E chdir build cmake .. -D OSX=ON -D RELEASE=$RELEASE -D CMAKE_TOOLCHAIN_FILE=./osx.cmake && make -C build
else
  cmake -E make_directory build
  cmake -E chdir build cmake -D RELEASE=$RELEASE .. && make -C build
fi
