#!/bin/bash

programName=`basename $0`

if [ $# -eq 0 ]; then
    echo "usage: ${programName} libraryDirectory"
else
    libraryDirectoryName=$1
    cd ${libraryDirectoryName}
    # construct regexp of library names
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
            newLibraryName="@loader_path/"`basename ${usedLibraryName}`
            install_name_tool \
                -change ${usedLibraryName} ${newLibraryName} \
                ${libraryName}
        done

        install_name_tool -id ${libraryName} ${libraryName}
    done
fi
