# -*- coding: utf-8 -*-
#
# Global Constants and functions in CMAKE for DrTT programs


# #################
# ### CONSTANTS ###
# #################

SET(WINDOWS ${WIN32})
SET(MACOS   ${APPLE})

IF(UNIX AND NOT APPLE)
    SET(LINUX 1)
ELSE()
    SET(LINUX 0)
ENDIF()

IF(WINDOWS)
    SET(GLOB_batchFileExtension "bat")
    SET(GLOB_nullDeviceName "NUL")
ELSEIF(LINUX)
    SET(GLOB_batchFileExtension "sh")
    SET(GLOB_nullDeviceName "/dev/null")
ELSE()
    SET(GLOB_batchFileExtension "sh")
    SET(GLOB_nullDeviceName "/dev/null")
ENDIF()

SET(GLOB_platformName
    ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
MESSAGE(STATUS "platformName = " ${GLOB_platformName})

#------------
# DIRECTORIES
#------------

# the subdirectory for the configuration used
SET(GLOB_configurationSubdirectory ${CMAKE_BUILD_TYPE})
SET(GLOB_buildDirectory ${CMAKE_CURRENT_BINARY_DIR})

IF("${GLOB_projectRootDirectory}" STREQUAL "")
    # no definition found, set it to relative directory
    CMAKE_PATH(SET GLOB_projectRootDirectory NORMALIZE
               ${CMAKE_CURRENT_SOURCE_DIR}/..)
ENDIF()

IF("${GLOB_targetDirectory}" STREQUAL "")
    # no definition found, set it to relative directory
    CMAKE_PATH(SET GLOB_targetDirectory NORMALIZE
               ${GLOB_projectRootDirectory}/_DISTRIBUTION)
ENDIF()

CMAKE_PATH(SET GLOB_platformTargetDirectory NORMALIZE
           ${GLOB_targetDirectory}/targetPlatforms/${GLOB_platformName})

SET(GLOB_platformTargetSubdirectory )
CMAKE_PATH(APPEND GLOB_platformTargetSubdirectory
           "${GLOB_platformTargetDirectory}"
           "${GLOB_configurationSubdirectory}")
MESSAGE(STATUS "platformTargetSubdirectory = "
        ${GLOB_platformTargetSubdirectory})
FILE(MAKE_DIRECTORY ${GLOB_platformTargetSubdirectory})

# --- documentation ---
IF("${GLOB_docDirectory}" STREQUAL "")
    # no definition found, set it to relative directory
    CMAKE_PATH(SET GLOB_docDirectory NORMALIZE
               ${GLOB_projectRootDirectory}/doc)
ENDIF()

CMAKE_PATH(SET GLOB_docLaTeXDirectory NORMALIZE
           ${GLOB_docDirectory}/latex)
CMAKE_PATH(SET GLOB_doxygenTargetDirectory NORMALIZE
           ${GLOB_projectRootDirectory}/internalDocumentation)

#------
# FILES
#------
       
# --- documentation file names ---
SET(GLOB_docLaTeXFileNameStem ${CMAKE_PROJECT_NAME}-documentation)
CMAKE_PATH(SET GLOB_docLaTeXFileName NORMALIZE
           ${GLOB_docLaTeXDirectory}/${GLOB_docLaTeXFileNameStem}.ltx)
CMAKE_PATH(SET GLOB_docLaTeXLocalFileName NORMALIZE
           ${GLOB_buildDirectory}/${GLOB_docLaTeXFileNameStem}.ltx)
SET(GLOB_docDoxygenFileName ${CMAKE_PROJECT_NAME}-doxygen-FULL.cfg)
CMAKE_PATH(SET GLOB_docDoxygenFileTemplate NORMALIZE
           ${GLOB_docDirectory}/doxygen/${GLOB_docDoxygenFileName}.in)

# --- Windows specific files ---
IF(MSVC)
    CMAKE_PATH(SET GLOB_debuggerVisualizationFileName NORMALIZE
               ${CMAKE_CURRENT_SOURCE_DIR}/Windows/datastructures.natvis)
ELSE()
    SET(GLOB_debuggerVisualizationFileName )
ENDIF()

# --- MacOS specific files ---
IF(MACOS)
    CMAKE_PATH(SET GLOB_macOSMiscDirectory NORMALIZE
               ${CMAKE_CURRENT_SOURCE_DIR}/MacOS)
    CMAKE_PATH(SET GLOB_plistFileTemplate NORMALIZE
               ${GLOB_macOSMiscDirectory}/Info.plist.in)
    SET(GLOB_nibFileName RecentFilesMenuTemplate.nib)
    CMAKE_PATH(SET GLOB_nibFileSourcePath NORMALIZE
               ${GLOB_macOSMiscDirectory}/${GLOB_nibFileName})
ENDIF()

# -----------------------------
# --- Apple bundle settings ---
# -----------------------------

IF(MACOS)
    # bundle settings
    SET(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    SET(GLOB_bundlePrefix eu.tensi.thomas.${CMAKE_PROJECT_NAME})

    # adaptation script for removing absolut paths in library
    # references
    CMAKE_PATH(SET dynamicLibraryAdaptationScript NORMALIZE
               ${GLOB_macOSMiscDirectory}/adaptLibraryFileLinkage.sh)
ENDIF(MACOS)

# #################
# ### FUNCTIONS ###
# #################

FUNCTION(GLOB_showList description listName)
    LIST(JOIN ${listName} "#" temp)
    MESSAGE(STATUS
            ${description} ": " ${listName} " = " ${temp})
ENDFUNCTION(GLOB_showList)

#=========
#= BUILD =
#=========

FUNCTION(GLOB_adaptLibraryFileLinkage
         originalDynamicLibrariesDirectory
         effectiveDynamicLibrariesDirectory
         dynamicLibraryNameList)
    # copies library files in MacOS given by <dynamicLibraryNameList>
    # from <originalDynamicLibrariesDirectory> to intermediate
    # directory <effectiveDynamicLibrariesDirectory> and changes
    # library path references in those libraries to a relative path

    IF("${dynamicLibraryNameList}" STRGREATER "")
        # copy files into intermediate directory
        FILE(MAKE_DIRECTORY ${effectiveDynamicLibrariesDirectory})
        SET(originalDynamicLibraryList ${dynamicLibraryNameList})
        LIST(TRANSFORM originalDynamicLibraryList
             PREPEND "${originalDynamicLibrariesDirectory}/")
        FILE(COPY ${originalDynamicLibraryList}
             DESTINATION ${effectiveDynamicLibrariesDirectory})

        # scan all files in intermediate directory and change path
        # references to "@loader_path"
        EXECUTE_PROCESS(COMMAND "bash"
                        ${dynamicLibraryAdaptationScript}
                        ${effectiveDynamicLibrariesDirectory})
    ENDIF()
ENDFUNCTION(GLOB_adaptLibraryFileLinkage)

#--------------------

FUNCTION(GLOB_makeTarget_commonProjectLibrary
         sourceFileList)
    # makes target for a library consisting of common sources for all
    # plugins from <sourceFileList>

    SET(targetName CommonProjectLibrary)

    ADD_LIBRARY(${targetName} STATIC EXCLUDE_FROM_ALL
                ${sourceFileList})

    CMPCONF_addCompilerFlags(${targetName} TRUE)

    SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                          POSITION_INDEPENDENT_CODE TRUE
                          INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
ENDFUNCTION(GLOB_makeTarget_commonProjectLibrary)

#--------------------

FUNCTION(GLOB_makeTarget_documentation
         targetName sourceFileList)
    # makes target named <targetName> for PDF documentation and
    # internal documentation from source files in <sourceFileList>

    SET(documentationTargetName ${targetName})
    ADD_CUSTOM_TARGET(${documentationTargetName} ALL)

    # -- LaTeX documentation --
    IF(NOT LATEX_PDFLATEX_FOUND)
        MESSAGE(STATUS "No PDFLaTeX compiler found --> skipping.")
    ELSE()
        SET(targetName pdfDocumentation)
        GLOB_makeTarget_documentationLaTeX(${targetName})
        ADD_DEPENDENCIES(${documentationTargetName} ${targetName})
    ENDIF()

    # -- DoxyGen documentation --
    IF(NOT DOXYGEN_FOUND)
        MESSAGE(STATUS "No DoxyGen or GraphViz found --> skipping.")
    ELSE()
        SET(targetName internalDocumentation)
        GLOB_makeTarget_documentationInternal(${targetName}
                                             ${sourceFileList})
        ADD_DEPENDENCIES(${documentationTargetName} ${targetName})
    ENDIF()
ENDFUNCTION(GLOB_makeTarget_documentation)

#--------------------

FUNCTION(GLOB_makeTarget_documentationInternal
         targetName allSrcFileList)
    # makes target for the DoxyGen documentation as HTML files

    CMAKE_PATH(SET doxygenRootFile NORMALIZE
               ${GLOB_doxygenTargetDirectory}/html/index.html)
    SET(DOXYGEN_SHOW_PRIVATE_FEATURES YES)
    CONFIGURE_FILE(${GLOB_docDoxygenFileTemplate} ${GLOB_docDoxygenFileName})

    ADD_CUSTOM_COMMAND(OUTPUT ${doxygenRootFile}
                       COMMAND ${DOXYGEN_EXECUTABLE} ${GLOB_docDoxygenFileName}
                       WORKING_DIRECTORY ${GLOB_buildDirectory}
                       COMMENT "Building DoxyGen API Documentation."
                       DEPENDS ${allSrcFileList})

    ADD_CUSTOM_TARGET(${targetName} DEPENDS ${doxygenRootFile})
ENDFUNCTION(GLOB_makeTarget_documentationInternal)

#--------------------

FUNCTION(GLOB_makeTarget_documentationLaTeX targetName)
    # makes a target for the LaTeX documentation as a PDF manual; does
    # a two phase compilation to get the TOC and AUX files correct

    SET(commonLaTeXParameters
        -interaction=nonstopmode
        --output-directory=${GLOB_buildDirectory})

    CMAKE_PATH(GET PDFLATEX_COMPILER PARENT_PATH teXFotCommand)
    CMAKE_PATH(APPEND teXFotCommand "texfot")

    IF(WINDOWS)
        SET(teXFotCommand ${teXFotCommand}.exe)
    ENDIF()

    IF(EXISTS ${teXFotCommand})
        SET(laTeXCommand
            ${teXFotCommand} --ignore erfull ${PDFLATEX_COMPILER})
    ELSE()
        SET(laTeXCommand ${PDFLATEX_COMPILER})
    ENDIF()

    SET(pdfLaTeXCommandA
        ${laTeXCommand} -draftmode ${commonLaTeXParameters})

    SET(pdfLaTeXCommandB
        ${laTeXCommand} ${commonLaTeXParameters})

    CMAKE_PATH(SET docLaTeXFileNameStemBuildPath NORMALIZE
               ${GLOB_buildDirectory}/${GLOB_docLaTeXFileNameStem})

    SET(docLaTeXPdfFileName ${docLaTeXFileNameStemBuildPath}.pdf)

    # make a temporary copy of documentation and figures into build
    # directory and then process files there via LaTeX compiler
    ADD_CUSTOM_COMMAND(OUTPUT  ${GLOB_docLaTeXLocalFileName}
                       DEPENDS ${GLOB_docLaTeXFileName}
                       COMMAND ${CMAKE_COMMAND} -E copy_directory
                               ${GLOB_docLaTeXDirectory}/figures
                               ${GLOB_buildDirectory}/figures
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different
                               ${GLOB_docLaTeXFileName}
                               ${GLOB_docLaTeXLocalFileName}
                       COMMENT "Copying LaTeX Files.")

    ADD_CUSTOM_COMMAND(OUTPUT  ${docLaTeXPdfFileName}
                       DEPENDS ${GLOB_docLaTeXLocalFileName}
                       COMMAND ${pdfLaTeXCommandA}
                               ${GLOB_docLaTeXFileNameStem}.ltx
                               >${GLOB_nullDeviceName}
                       COMMAND ${pdfLaTeXCommandB}
                               ${GLOB_docLaTeXFileNameStem}.ltx
                       COMMENT "Generating Manual.")

    ADD_CUSTOM_TARGET(${targetName}
                      DEPENDS ${docLaTeXPdfFileName})
ENDFUNCTION(GLOB_makeTarget_documentationLaTeX)

#================
#= INSTALLATION =
#================

FUNCTION(GLOB_install_documentationPDF)
    # installs the PDF file generated into documentation directory

    IF(LATEX_PDFLATEX_FOUND)
        # WORKAROUND: find the path of the PDF file from some target
        CMAKE_PATH(SET specificTargetDirectory NORMALIZE
                   ${GLOB_targetDirectory}/doc)
        FILE(REMOVE_RECURSE ${specificTargetDirectory})
        INSTALL(FILES ${GLOB_buildDirectory}/${GLOB_docLaTeXFileNameStem}.pdf
                DESTINATION ${specificTargetDirectory})
    ENDIF()
ENDFUNCTION(GLOB_install_documentationPDF)

#--------------------

FUNCTION(GLOB_install_testFiles regressionTestFileList)
    # installs all files in <regressionTestFileList> into test
    # directory

    CMAKE_PATH(SET specificTargetDirectory NORMALIZE
               ${GLOB_targetDirectory}/test)
    FILE(REMOVE_RECURSE ${specificTargetDirectory})

    FOREACH(testFile ${regressionTestFileList})
        INSTALL(FILES ${testFile}
            DESTINATION ${specificTargetDirectory})
    ENDFOREACH()
ENDFUNCTION(GLOB_install_testFiles)
