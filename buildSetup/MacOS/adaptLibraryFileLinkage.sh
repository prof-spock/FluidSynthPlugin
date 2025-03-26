#!/bin/bash
# script for MacOS to adapt linkage paths in dynamic libraries
# to containing directory
#
# $1: libraryDirectoryName  name of directory with dynamic libraries
#                           to be adapted

programName=`basename $0`

if [ $# -eq 0 ]; then
    echo "usage: ${programName} libraryDirectory"
else
    libraryDirectoryName=$1
    cd ${libraryDirectoryName}

    # construct regexp of library names to be matched in list produced
    # by otool -L
    libraryNameRegexp=`ls \
                       | sed "s/[^a-zA-Z].*//" \
                       | tr -s '\n' '|' \
                       | sed "s/|$//"`

    for libraryName in *.dylib; do
        # extract all partner library names
        usedLibraryNameList=`otool -L ${libraryName} \
                             | grep -v "${libraryName}" \
                             | egrep "${libraryNameRegexp}" \
                             | sed "s/[^[^/]*//" \
                             | sed "s/ .*//"`

        for usedLibraryName in ${usedLibraryNameList}; do
            # remove path prefix and replace by "@loader_path/"
            newLibraryName="@loader_path/"`basename ${usedLibraryName}`
            install_name_tool \
                -change ${usedLibraryName} ${newLibraryName} \
                ${libraryName}
        done

        # set identification of library to plain file name
        install_name_tool -id ${libraryName} ${libraryName}
    done
fi
