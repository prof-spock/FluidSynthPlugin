# -*- coding: utf-8 -*-
#
# Global Constants and functions in CMAKE for DrTT JUCE plugins


# #################
# ### CONSTANTS ###
# #################

SET(WINDOWS ${WIN32})
SET(MACOSX  ${APPLE})

IF(UNIX AND NOT APPLE)
    SET(LINUX 1)
ELSE()
    SET(LINUX 0)
ENDIF()

IF(WINDOWS)
    SET(GLOB_batchFileExtension "bat")
    SET(GLOB_juceFileExtension cpp)
    SET(GLOB_nullDeviceName "NUL")
    SET(GLOB_vst3LibraryNameExtension ".vst3")
    SET(GLOB_vst3PlatformName "x86_64-win")
ELSEIF(LINUX)
    SET(GLOB_batchFileExtension "sh")
    SET(GLOB_juceFileExtension cpp)
    SET(GLOB_nullDeviceName "/dev/null")
    SET(GLOB_vst3LibraryNameExtension ".so")
    SET(GLOB_vst3PlatformName "x86_64-linux")
ELSE()
    SET(GLOB_batchFileExtension "sh")
    SET(GLOB_juceFileExtension mm)
    SET(GLOB_nullDeviceName "/dev/null")
    SET(GLOB_vst3LibraryNameExtension ".so")
    SET(GLOB_vst3PlatformName "x86_64-macosx")
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
CMAKE_PATH(SET GLOB_targetDirectory NORMALIZE
           ${CMAKE_CURRENT_SOURCE_DIR}/../_DISTRIBUTION)
CMAKE_PATH(SET GLOB_platformTargetDirectory NORMALIZE
           ${GLOB_targetDirectory}/targetPlatforms)

SET(GLOB_platformTargetSubdirectory )
CMAKE_PATH(APPEND GLOB_platformTargetSubdirectory
           "${GLOB_platformTargetDirectory}"
           "${GLOB_platformName}"
           "${GLOB_configurationSubdirectory}")
MESSAGE(STATUS "platformTargetSubdirectory = "
        ${GLOB_platformTargetSubdirectory})
FILE(MAKE_DIRECTORY ${GLOB_platformTargetSubdirectory})

# --- documentation ---
CMAKE_PATH(SET GLOB_docDirectory NORMALIZE
           ${CMAKE_CURRENT_SOURCE_DIR}/../doc)
CMAKE_PATH(SET GLOB_docLaTeXDirectory NORMALIZE
           ${GLOB_docDirectory}/latex)
CMAKE_PATH(SET GLOB_doxygenTargetDirectory NORMALIZE
           ${CMAKE_CURRENT_SOURCE_DIR}/../internalDocumentation)

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

# --- MacOSX specific files ---
IF(MACOSX)
    CMAKE_PATH(SET GLOB_macOSMiscDirectory NORMALIZE
               ${CMAKE_CURRENT_SOURCE_DIR}/MacOSX)
    CMAKE_PATH(SET GLOB_plistFileTemplate NORMALIZE
               ${GLOB_macOSMiscDirectory}/Info.plist.in)
    SET(GLOB_nibFileName RecentFilesMenuTemplate.nib)
    CMAKE_PATH(SET GLOB_nibFileSourcePath NORMALIZE
               ${GLOB_macOSMiscDirectory}/${GLOB_nibFileName})
ENDIF()

# -------------------------------------------------
# --- Apple framework files and bundle settings ---
# -------------------------------------------------

IF(NOT MACOSX)
    SET(GLOB_frameworkNameList )
ELSE()
    # included frameworks
    LIST(APPEND GLOB_frameworkNameList
         Accelerate AppKit AudioToolbox AudioUnit Carbon
         Cocoa CoreAudio CoreAudioKit CoreMIDI
         DiscRecording Foundation IOKit Metal MetalKit
         QuartzCore Security WebKit
    )

    # bundle settings
    SET(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    SET(GLOB_bundlePrefix eu.tensi.thomas.${CMAKE_PROJECT_NAME})

    # adaptation script for removing absolut paths in library
    # references
    CMAKE_PATH(SET dynamicLibraryAdaptationScript NORMALIZE
               ${GLOB_macOSMiscDirectory}/adaptLibraryFileLinkage.sh)
ENDIF(NOT MACOSX)

# #################
# ### FUNCTIONS ###
# #################

#=========
#= BUILD =
#=========

FUNCTION(GLOB_adaptLibraryFileLinkage
         originalDynamicLibrariesDirectory
         effectiveDynamicLibrariesDirectory
         dynamicLibraryNameList)
    # copies library files in MacOSX given by <dynamicLibraryNameList>
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

FUNCTION(GLOB_addPluginNibFileForAUTarget targetName resourcePath)
    # adds a NIB file into resource folder <resourcePath> for target
    # <targetName> representing an audio unit library

    CMAKE_PATH(SET targetNibFileName NORMALIZE
               ${resourcePath}/${GLOB_nibFileName})
    ADD_CUSTOM_COMMAND(TARGET ${targetName} POST_BUILD
                       COMMAND ibtool
                               --strip ${targetNibFileName}
                               --output-format human-readable-text
                               ${GLOB_nibFileSourcePath})
ENDFUNCTION(GLOB_addPluginNibFileForAUTarget)

#--------------------

FUNCTION(GLOB_makeTarget_commonProjectLibrary
         sourceFileList)
    # makes target for a library consisting of common sources for all
    # plugins from <sourceFileList>

    SET(targetName CommonProjectLibrary)

    ADD_LIBRARY(${targetName} STATIC EXCLUDE_FROM_ALL
                ${sourceFileList})

    addCompilerFlags(${targetName} TRUE)

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

#--------------------

FUNCTION(GLOB_makeTarget_juceFramework
         jucePrefix)
    # makes a JUCE static library target named <JuceFramework> with
    # classes from the JUCE modules directory

    # --- the file name list of facade files for JUCE; those files ---
    # --- reference real implementations in JUCE; note that on     ---
    # --- Apple platforms the Objective-C++ (.mm) files are used   ---
    # --- instead of the standard C++ (.cpp) files                 ---
    SET(srcJuceFacadeFileList )
    LIST(APPEND srcJuceFacadeFileList
         ${jucePrefix}_audio_basics.${GLOB_juceFileExtension}
         ${jucePrefix}_audio_devices.${GLOB_juceFileExtension}
         ${jucePrefix}_audio_formats.${GLOB_juceFileExtension}
         ${jucePrefix}_audio_processors.${GLOB_juceFileExtension}
         ${jucePrefix}_audio_utils.${GLOB_juceFileExtension}
         ${jucePrefix}_core.${GLOB_juceFileExtension}
         ${jucePrefix}_data_structures.${GLOB_juceFileExtension}
         ${jucePrefix}_events.${GLOB_juceFileExtension}
         ${jucePrefix}_graphics.${GLOB_juceFileExtension}
         ${jucePrefix}_gui_basics.${GLOB_juceFileExtension}
         ${jucePrefix}_gui_extra.${GLOB_juceFileExtension}
    )

    IF(MACOSX)
        LIST(APPEND srcJuceFacadeFileList
             ${jucePrefix}_audio_ausdk_base.cpp
             ${jucePrefix}_audio_ausdk_buffer.cpp
             ${jucePrefix}_audio_ausdk_bufferallocator.cpp
             ${jucePrefix}_audio_ausdk_componentbase.cpp
             ${jucePrefix}_audio_ausdk_effectbase.cpp
             ${jucePrefix}_audio_ausdk_inputelement.cpp
             ${jucePrefix}_audio_ausdk_midibase.cpp
             ${jucePrefix}_audio_ausdk_midieffectbase.cpp
             ${jucePrefix}_audio_ausdk_musicdevicebase.cpp
             ${jucePrefix}_audio_ausdk_outputelement.cpp
             ${jucePrefix}_audio_ausdk_plugindispatch.cpp
             ${jucePrefix}_audio_ausdk_scopeelement.cpp
        )
    ENDIF(MACOSX)

    SET(targetName JuceFramework)

    ADD_LIBRARY(${targetName} STATIC EXCLUDE_FROM_ALL
                ${srcJuceFacadeFileList})

    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC
                               ${additionalIncludeDirectoryList})

    # do not check the JUCE library functions at all
    addCompilerFlags(${targetName} FALSE)

    SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                          POSITION_INDEPENDENT_CODE TRUE
                          INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
ENDFUNCTION(GLOB_makeTarget_juceFramework)

#--------------------

FUNCTION(GLOB_makeTarget_pluginAU
         targetName pluginName libraryName
         auPluginName auLongPluginName auMainType auSubType
         sourceFileList
         includeDirectoryList
         libraryList
)
    # makes a target for an AU plugin named <targetName> with the AU
    # wrapper classes combined with the static library for given
    # effect named <pluginName>; <auMainType> and <auSubType> gives
    # the AU type and subtype code for the plugin; <sourceFileList>
    # tells the list of sources needed, <includeDirectoryList> the
    # list of additional include directories and <libraryList> the
    # list of statically linked libraries

    IF(MACOSX)
        SET(plistFileName ${pluginName}_AppleInfo.plist)
        SET(principalClassName "NSApplication")

        ADD_LIBRARY(${targetName} MODULE ${sourceFileList})

        addCompilerFlags(${targetName} TRUE)

        TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC
                                   ${includeDirectoryList})

        TARGET_LINK_LIBRARIES(${targetName}
                              ${libraryList})

        # add Apple frameworks to library list
        FOREACH(frameworkName ${GLOB_frameworkNameList})
          TARGET_LINK_LIBRARIES(${targetName}
                                "-framework ${frameworkName}")
        ENDFOREACH(frameworkName)

        # make the specific PLIST file
        SET(JUCE_manufacturerName "DrTT")
        SET(JUCE_auPluginName     ${auPluginName})
        SET(JUCE_auLongPluginName ${auLongPluginName})
        SET(JUCE_auMainType       ${auMainType})
        SET(JUCE_auSubType        ${auSubType})
        SET(JUCE_bundlePrefix     ${GLOB_bundlePrefix})
        CONFIGURE_FILE(${GLOB_plistFileTemplate} ${plistFileName})

        SET_TARGET_PROPERTIES(${targetName} PROPERTIES
            BUNDLE_EXTENSION component
            BUNDLE TRUE
            XCODE_ATTRIBUTE_WRAPPER_EXTENSION component
            XCODE_ATTRIBUTE_LIBRARY_STYLE Bundle
            XCODE_ATTRIBUTE_GENERATE_PKGINFO_FILE YES
            MACOSX_BUNDLE_BUNDLE_NAME ${libraryName}
            MACOSX_BUNDLE_GUI_IDENTIFIER ${GLOB_bundlePrefix}.${libraryName}
            MACOSX_BUNDLE_INFO_PLIST ${plistFileName}
            OUTPUT_NAME ${libraryName}
        )

        # add resources for audio unit library
        CMAKE_PATH(SET resourcePath NORMALIZE
                   ${libraryName}.component/Contents/Resources)
        ADD_CUSTOM_COMMAND(TARGET ${targetName} POST_BUILD
                           COMMAND mkdir -p ${resourcePath})
        GLOB_addPluginNibFileForAUTarget(${targetName} ${resourcePath})
    ENDIF(MACOSX)
ENDFUNCTION(GLOB_makeTarget_pluginAU)

#--------------------

FUNCTION(GLOB_makeTarget_pluginStaticLib
         targetName pluginDirectory fileList)
    # makes a static library target named <targetName> with the engine
    # classes for given plugin named <pluginName>

    ADD_LIBRARY(${targetName} STATIC ${fileList})

    addCompilerFlags(${targetName} TRUE)

    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC ${pluginDirectory})

    SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                          POSITION_INDEPENDENT_CODE TRUE
                          INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
ENDFUNCTION(GLOB_makeTarget_pluginStaticLib)

#--------------------

FUNCTION(GLOB_makeTarget_pluginVST
         targetName pluginName libraryName
         sourceFileList includeDirectoryList libraryList
         unixLibraryList)
    # makes an effect VST plugin target named <targetName> with the
    # VST wrapper classes combined with the static library for given
    # effect named <pluginName>; <sourceFileList> tells the list of
    # sources needed, <includeDirectoryList> the list of additional
    # include directories and <libraryList> the list of statically
    # linked libraries; for Unix the <unixLibraryList> is additionally
    # provided

    IF(MACOSX)
        # on Apple platform a VST plugin is a library bundle
        ADD_LIBRARY(${targetName} MODULE
                    ${GLOB_debuggerVisualizationFileName}
                    ${sourceFileList})
    ELSE()
        # VST plugin is a dynamic library
        ADD_LIBRARY(${targetName} SHARED
                    ${GLOB_debuggerVisualizationFileName}
                    ${sourceFileList})
    ENDIF()

    addCompilerFlags(${targetName} TRUE)

    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC ${includeDirectoryList})
    TARGET_LINK_LIBRARIES(${targetName} ${libraryList})

    IF(MACOSX)
        # add framework libraries
        FOREACH(frameworkName ${GLOB_frameworkNameList})
            TARGET_LINK_LIBRARIES(${targetName}
                                  "-framework ${frameworkName}")
        ENDFOREACH(frameworkName)
    ENDIF(MACOSX)

    IF(LINUX)
        TARGET_LINK_LIBRARIES(${targetName} ${unixLibraryList})
        TARGET_LINK_LIBRARIES(${targetName} dl pthread rt)
    ENDIF(LINUX)

    # put library into a bundle structure regardless of operating
    # system
    IF(MACOSX)
        SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                              BUNDLE TRUE
                              BUNDLE_EXTENSION vst3
                              OUTPUT_NAME ${libraryName}
                              SUFFIX "")
    ELSE()
        SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                              OUTPUT_NAME ${libraryName}
                              PREFIX ""
                              SUFFIX ${GLOB_vst3LibraryNameExtension})

        IF(LINUX)
            CMAKE_PATH(SET libraryFileDirectory NORMALIZE
                       ${GLOB_buildDirectory})
        ELSE()
            CMAKE_PATH(SET libraryFileDirectory NORMALIZE
                       ${GLOB_buildDirectory}/${GLOB_configurationSubdirectory})
        ENDIF()

        CMAKE_PATH(SET libraryFileName NORMALIZE
                   ${libraryFileDirectory}/${libraryName}${GLOB_vst3LibraryNameExtension})

        SET(vst3Directory )
        CMAKE_PATH(APPEND vst3Directory
                   "${GLOB_buildDirectory}"
                   "${libraryName}.vst3"
                   "Contents"
                   "${GLOB_vst3PlatformName}")
        
        ADD_CUSTOM_COMMAND(TARGET ${targetName} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory
                                   ${vst3Directory}
                           COMMAND ${CMAKE_COMMAND} -E copy
                                   ${libraryFileName}
                                   ${vst3Directory}
                                   COMMENT "Bundling ${libraryName}.")
    ENDIF()
ENDFUNCTION(GLOB_makeTarget_pluginVST)

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
    
FUNCTION(GLOB_install_pluginAU libraryName dynamicLibraryList)
    # install AU plugin given by <libraryName> together with dynamic
    # libraries in <dynamicLibraryList>

    IF(MACOSX)
        CMAKE_PATH(SET targetAUDirectory NORMALIZE
                   ${GLOB_platformTargetSubdirectory}/AU)
        SET(targetContentsAUDirectory )
        CMAKE_PATH(APPEND targetAUContentsDirectory
                   ${targetAUDirectory}
                   ${libraryName}.component
                   "Contents"
                   "MacOS"
        )

        FILE(REMOVE_RECURSE ${targetAUDirectory})
        SET(targetName ${libraryName}_AU)
        INSTALL(TARGETS ${targetName}
                DESTINATION ${targetAUDirectory})
        INSTALL(FILES ${dynamicLibraryList}
                DESTINATION ${targetAUContentsDirectory})
    ENDIF(MACOSX)
ENDFUNCTION(GLOB_install_pluginAU)

#--------------------

FUNCTION(GLOB_install_pluginVST libraryName dynamicLibraryList)
    # install VST plugin given by <libraryName> together with dynamic
    # libraries in <dynamicLibraryList>

    CMAKE_PATH(SET targetVST3Directory NORMALIZE
               ${GLOB_platformTargetSubdirectory}/VST3)
    FILE(REMOVE_RECURSE ${targetVST3Directory})

    IF(NOT MACOSX)
        CMAKE_PATH(SET targetVST3Path NORMALIZE
                   ${targetVST3Directory}/${libraryName}.vst3)
        INSTALL(DIRECTORY ${GLOB_buildDirectory}/${libraryName}.vst3/Contents
                DESTINATION ${targetVST3Path})
        INSTALL(FILES ${dynamicLibraryList}
                DESTINATION ${targetVST3Path}/Contents/${GLOB_vst3PlatformName})
    ELSE()
        SET(targetName ${libraryName}_VST)
        INSTALL(TARGETS ${targetName}
                DESTINATION ${targetVST3Directory})
        INSTALL(FILES ${dynamicLibraryList}
                DESTINATION ${targetVST3Directory}/${libraryName}.vst3/Contents/MacOS)
    ENDIF()
ENDFUNCTION(GLOB_install_pluginVST)

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
