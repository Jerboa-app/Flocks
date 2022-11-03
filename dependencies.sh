#!/bin/sh
cd include
if [ -d SFML-2.5.1 ];
then
	echo "Directory $DEP exists, overwrite?"
	read -p "[y/n]: " I
	if [ "$I" = "n" ];
	then
		echo "Quitting"
		exit
	fi
fi

rm -rf SFML-2.5.1
tar -xvf SFML-2.5.1.tar.gz
cd SFML-2.5.1
mkdir build && cd build
cmake .. -D BUILD_SHARED_LIBS=FALSE && make
export SFML_DIR="../../SFML-2.5.1/build"

cd ..
mkdir build-windows && cd build-windows
cmake .. -D BUILD_SHARED_LIBS=FALSE -D CMAKE_TOOLCHAIN_FILE=../../../windows.cmake && make
