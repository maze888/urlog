#!/bin/bash

CMAKE="cmake CMakeLists.txt"

function distclean_cmake()
{
	for fld in $(find -name "CMakeLists.txt" -printf '%h ')
	do
		for cmakefile in CMakeCache.txt cmake_install.cmake CTestTestfile.cmake CMakeFiles Makefile install_manifest.txt _deps
			do
				rm -rf $fld/$cmakefile
			done
	done
}

distclean_cmake && $CMAKE

exit 0
