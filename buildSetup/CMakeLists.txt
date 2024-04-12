###############################################
# CMAKE file for the FluidSynthPlugin project #
#                                             #
# Dr. Thomas Tensi, 2022-04                   #
###############################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(FluidSynthPlugin
        VERSION 1.0
        LANGUAGES CXX)

# =============================
# === program configuration ===
# =============================

SET(WINDOWS ${WIN32})
SET(MACOSX  ${APPLE})

IF(UNIX AND NOT APPLE)
    SET(LINUX 1)
ELSE()
    SET(LINUX 0)
ENDIF()

SET(platformName ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
MESSAGE(STATUS "platformName = " ${platformName})

IF(WINDOWS)
    SET(libraryNameExtension ".vst3")
    SET(vst3PlatformName "x86_64-win")
ELSEIF(LINUX)
    SET(libraryNameExtension ".so")
    SET(vst3PlatformName "x86_64-linux")
ELSE()
    SET(libraryNameExtension ".so")
    SET(vst3PlatformName "x86_64-macosx")
ENDIF()

# --- compiler ---
INCLUDE(PluginCPPCompilerSettings.cmake)
INCLUDE(CPPCompilerConfiguration.cmake)

# --- LaTeX ---
FIND_PACKAGE(LATEX QUIET OPTIONAL_COMPONENTS PDFLATEX)

# --- DoxyGen ---
FIND_PACKAGE(Doxygen QUIET)

# --- JUCE ---
FIND_PACKAGE(juce QUIET)

# ===========================
# === local configuration ===
# ===========================

# local configuration settings
INCLUDE(LocalConfiguration.cmake)

# the name stems of all plugins to produce (currently only a single
# one)
LIST(APPEND pluginNameList FluidSynthPlugin)

# the name of the file converter
SET(converterName FluidSynthFileConverter)

# the subdirectory for the configuration used
SET(configurationSubdirectory ${CMAKE_BUILD_TYPE})

# ===================
# === directories ===
# ===================

SET(buildDirectory ${CMAKE_CURRENT_BINARY_DIR})
SET(targetDirectory ${CMAKE_CURRENT_SOURCE_DIR}/../_DISTRIBUTION)
SET(platformTargetDirectory ${targetDirectory}/targetPlatforms)

# --- source directories ---
SET(srcDirectory ${CMAKE_CURRENT_SOURCE_DIR}/../src)
SET(srcAudioSupportDirectory       ${srcDirectory}/Audio)
SET(srcBaseModulesDirectory        ${srcDirectory}/BaseModules)
SET(srcBaseTypesDirectory          ${srcDirectory}/BaseTypes)
SET(srcContainersDirectory         ${srcBaseTypesDirectory}/Containers)
SET(srcGenericTypesDirectory       ${srcBaseTypesDirectory}/GenericTypes)
SET(srcPrimitivesDirectory         ${srcBaseTypesDirectory}/Primitives)
SET(srcLibrariesDirectory          ${srcDirectory}/Libraries)
SET(srcFluidSynthLibDirectory      ${srcDirectory}/Libraries/FluidSynth)
SET(srcJuceFacadeDirectory         ${srcDirectory}/Libraries/JuceFacade)
SET(srcMidiSupportDirectory        ${srcDirectory}/MIDI)
SET(srcMainRootDirectory           ${srcDirectory}/Main)

# directories for the plugins (currently only a single one) and the
# player
SET(srcFluidSynthPluginDirectory    ${srcMainRootDirectory}/FluidSynthPlugin)
SET(srcFluidSynthConverterDirectory ${srcMainRootDirectory}/${converterName})

SET(originalDynamicLibrariesDirectory
    ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/${platformName})

IF(NOT MACOSX)
    SET(dynamicLibrariesDirectory ${originalDynamicLibrariesDirectory})
ELSE()
    SET(dynamicLibrariesDirectory ${buildDirectory}/libraries)
ENDIF()

SET(docDirectory ${CMAKE_CURRENT_SOURCE_DIR}/../doc)
SET(docLaTeXDirectory ${docDirectory}/latex)
SET(doxygenTargetDirectory
    ${CMAKE_CURRENT_SOURCE_DIR}/../internalDocumentation)
SET(testFileDirectory ${CMAKE_CURRENT_SOURCE_DIR}/../test)

# --- include directories ---

SET(juceModulesDirectory ${JUCE_DIR}/modules)

LIST(APPEND standardIncludeDirectoryList
     ${srcAudioSupportDirectory}
     ${srcBaseModulesDirectory}
     ${srcContainersDirectory}
     ${srcGenericTypesDirectory}
     ${srcPrimitivesDirectory}
     ${srcLibrariesDirectory}
     ${srcMainRootDirectory}
     ${srcMidiSupportDirectory}
     ${srcFluidSynthLibDirectory}
)

LIST(APPEND additionalIncludeDirectoryList
     ${srcJuceFacadeDirectory}
     ${juceModulesDirectory}
     ${juceModulesDirectory}/juce_audio_plugin_client/AU
     ${juceModulesDirectory}/juce_audio_processors/format_types/VST3_SDK
)

# --- additional JUCE settings ---

SET(jucePrefix ${srcJuceFacadeDirectory}/include_juce)

IF(MACOSX)
    SET(juceFileExtension mm)
ELSE()
    SET(juceFileExtension cpp)
ENDIF()

# --- add dynamic linkage library for MacOS and Unix ---
IF(NOT WINDOWS)
    LINK_LIBRARIES(${CMAKE_DL_LIBS})
ENDIF(NOT WINDOWS)

# --- add include directories and libraries for Unix ---
IF(LINUX)
    FIND_PACKAGE(PkgConfig REQUIRED) 
    LIST(APPEND relevantPackageList freetype2)
    PKG_CHECK_MODULES(linuxPackages ${relevantPackageList})
    LIST(APPEND unixIncludeDirectoryList /usr/include/freetype2)
    LIST(APPEND unixLibraryList ${linuxPackages_LINK_LIBRARIES})

    LIST(APPEND additionalIncludeDirectoryList
         ${unixIncludeDirectoryList}
    )
ENDIF(LINUX)

# ====================
# === source files ===
# ====================

LIST(APPEND srcAudioModulesFileList
     ${srcAudioSupportDirectory}/AudioSampleFifoQueue.cpp
     ${srcAudioSupportDirectory}/AudioSampleFifoQueueVector.cpp
     ${srcAudioSupportDirectory}/AudioSampleList.cpp
     ${srcAudioSupportDirectory}/AudioSampleListVector.cpp
)

LIST(APPEND srcBaseModulesFileList
    ${srcBaseModulesDirectory}/Environment.cpp
    ${srcBaseModulesDirectory}/File.cpp
    ${srcBaseModulesDirectory}/LoggingSupport.cpp
    ${srcBaseModulesDirectory}/OperatingSystem.cpp
    ${srcBaseModulesDirectory}/StringUtil.cpp
)

LIST(APPEND srcContainersFileList
     ${srcContainersDirectory}/ByteList.cpp
     ${srcContainersDirectory}/Dictionary.cpp
     ${srcContainersDirectory}/NaturalList.cpp
     ${srcContainersDirectory}/StringList.cpp
)

LIST(APPEND srcPrimitivesFileList
     ${srcPrimitivesDirectory}/MyString.cpp
)

LIST(APPEND srcFluidSynthMidiSupportFileList
     ${srcMidiSupportDirectory}/MidiEvent.cpp
     ${srcMidiSupportDirectory}/MidiEventList.cpp
     ${srcMidiSupportDirectory}/MidiEventConverter.cpp
     ${srcMidiSupportDirectory}/MidiPresetIdentification.cpp
)

LIST(APPEND srcFluidSynthLibraryFileList
     ${srcLibrariesDirectory}/DynamicLibrary.cpp
     ${srcFluidSynthLibDirectory}/FluidSynth.cpp
     ${srcFluidSynthLibDirectory}/FluidSynthSettings.cpp
     ${srcFluidSynthLibDirectory}/FluidSynthSoundFont.cpp
     ${srcFluidSynthLibDirectory}/FluidSynthSynthesizer.cpp
)

LIST(APPEND commonSrcFileList
     ${srcAudioModulesFileList}
     ${srcBaseModulesFileList}
     ${srcContainersFileList}
     ${srcPrimitivesFileList}
     ${srcFluidSynthMidiSupportFileList}
     ${srcFluidSynthLibraryFileList}
)

LIST(APPEND allSrcFileList ${commonSrcFileList})

#---------------------------------------------------------------------
# --- XXXFileList_VST is the file name list for the VST3i library  ---
# --- for some plugin                                              ---
# --------------------------------------------------------------------

FOREACH(pluginName ${pluginNameList})
    SET(effectDirectory ${src${pluginName}Directory})

    SET(fileListName src${pluginName}FileList_VST)

    LIST(APPEND ${fileListName}
         ${commonSrcFileList}
         ${srcMainRootDirectory}/BuildInformation.cpp
         ${effectDirectory}/${pluginName}_Editor.cpp
         ${effectDirectory}/${pluginName}_EditorSupport.cpp
         ${effectDirectory}/${pluginName}_EventProcessor.cpp
         ${effectDirectory}/JucePlugin-VST_1.${juceFileExtension}
    )
    
    IF(WINDOWS)
        LIST(APPEND ${fileListName}
             ${effectDirectory}/resources.rc
        )
    ENDIF(WINDOWS)

    LIST(APPEND allSrcFileList ${${fileListName}})
ENDFOREACH(pluginName)

# -----------------------------------------------------------------
# --- XXXFileList_AU is the file name list for the AU library   ---
# --- for some plugin                                           ---
# -----------------------------------------------------------------

IF(MACOSX)
    FOREACH(pluginName ${pluginNameList})
        SET(effectDirectory ${src${pluginName}Directory})

        SET(fileListName src${pluginName}FileList_AU)

        LIST(APPEND ${fileListName}
             ${commonSrcFileList}
             ${srcMainRootDirectory}/BuildInformation.cpp
             ${effectDirectory}/${pluginName}_EventProcessor.cpp
             ${effectDirectory}/${pluginName}_Editor.cpp
             ${effectDirectory}/${pluginName}_EditorSupport.cpp
             ${effectDirectory}/JucePlugin-util.cpp
             ${effectDirectory}/JucePlugin-AU_1.mm
             ${effectDirectory}/JucePlugin-AU_2.mm
             ${effectDirectory}/JucePlugin-Standalone.cpp
        )

        SET(allSrcFileList ${allSrcFileList} ${${fileListName}})
    ENDFOREACH(pluginName)
ENDIF(MACOSX)

# -------------------------------------------------------------------
# --- the list of dynamic libraries per platform                  ---
# -------------------------------------------------------------------

IF(WINDOWS)
    LIST(APPEND dynamicLibraryNameList
         libfluidsynth-3.dll libgcc_s_sjlj-1.dll libglib-2.0-0.dll
         libgobject-2.0-0.dll libgomp-1.dll libgthread-2.0-0.dll
         libinstpatch-2.dll libintl-8.dll libsndfile-1.dll libstdc++-6.dll
         libwinpthread-1.dll
    )
ELSEIF(MACOSX)
    LIST(APPEND dynamicLibraryNameList
         libFLAC.8.dylib libffi.8.dylib libfluidsynth.3.dylib
         libglib-2.0.0.dylib libgobject-2.0.0.dylib libgthread-2.0.0.dylib
         libiconv.2.dylib libintl.8.dylib libncurses.6.dylib
         libogg.0.dylib libopus.0.dylib libpcre.1.dylib
         libreadline.8.dylib libsndfile.1.dylib libvorbis.0.dylib
         libvorbisenc.2.dylib
    )
ELSEIF(LINUX)
    LIST(APPEND dynamicLibraryNameList
        libfluidsynth.so.3
    )
ENDIF()
 
LIST(APPEND dynamicLibraryList ${dynamicLibraryNameList})
LIST(TRANSFORM dynamicLibraryList PREPEND ${dynamicLibrariesDirectory}/)

# -------------------------------------------------------------------
# --- the file name list of facade files for JUCE; those files    ---
# --- reference real implementations in JUCE; note that on        ---
# --- Apple platforms the Objective-C++ (.mm) files are used      ---
# --- of the standard C++ (.cpp) files                            ---
# -------------------------------------------------------------------

LIST(APPEND srcJuceFacadeFileListSTD
     ${jucePrefix}_audio_basics.${juceFileExtension}
     ${jucePrefix}_audio_devices.${juceFileExtension}
     ${jucePrefix}_audio_formats.${juceFileExtension}
     ${jucePrefix}_audio_processors.${juceFileExtension}
     ${jucePrefix}_audio_utils.${juceFileExtension}
     ${jucePrefix}_core.${juceFileExtension}
     ${jucePrefix}_data_structures.${juceFileExtension}
     ${jucePrefix}_events.${juceFileExtension}
     ${jucePrefix}_graphics.${juceFileExtension}
     ${jucePrefix}_gui_basics.${juceFileExtension}
     ${jucePrefix}_gui_extra.${juceFileExtension} )

# --- documentation sources ---
SET(docLaTeXFileNameStem FluidSynthPlugin-documentation)
SET(docLaTeXFileName ${docLaTeXDirectory}/${docLaTeXFileNameStem}.ltx)
SET(docLaTeXLocalFileName ${buildDirectory}/${docLaTeXFileNameStem}.ltx)
SET(docDoxygenFileTemplate
    ${docDirectory}/doxygen/FluidSynthPlugin-doxygen-FULL.cfg.in)
SET(docDoxygenFileName FluidSynthPlugin-doxygen-FULL.cfg)

# --- regression test sources ---
IF(WINDOWS)
    LIST(APPEND regressionTestFileList
        ${testFileDirectory}/makeTestFiles.bat
    )
ELSE()
    CONFIGURE_FILE(${testFileDirectory}/makeTestFiles.sh.in
                   ${buildDirectory}/makeTestFiles.sh
                   @ONLY)
    LIST(APPEND regressionTestFileList
         ${buildDirectory}/makeTestFiles.sh
    )
ENDIF()

LIST(APPEND regressionTestFileList
     ${regressionTestFileList}
     ${testFileDirectory}/test_bass.mid
     ${testFileDirectory}/test_drums.mid
     ${testFileDirectory}/test_keyboard.mid
     ${testFileDirectory}/SimpleTestSoundfont.sf2
     ${testFileDirectory}/testFluidSynth-44100.rpp
     ${testFileDirectory}/testFluidSynth-48000.rpp
     ${testFileDirectory}/testFluidSynth-48000-singleVoice.rpp
)

# --- Windows specific files ---
IF(MSVC)
    SET(debuggerVisualizationFileName
        ${CMAKE_CURRENT_SOURCE_DIR}/Windows/datastructures.natvis)
ELSE()
    SET(debuggerVisualizationFileName )
ENDIF()

# --- MacOSX specific files ---
SET(macOSMiscDirectory ${CMAKE_CURRENT_SOURCE_DIR}/MacOSX)
SET(plistFileTemplate ${macOSMiscDirectory}/Info.plist.in)
SET(nibFileName RecentFilesMenuTemplate.nib)
SET(nibFileSourcePath ${macOSMiscDirectory}/${nibFileName})
SET(dynamicLibraryAdaptationScript
    ${macOSMiscDirectory}/adaptLibraryFileLinkage.sh)

# -------------------------------------------------
# --- Apple framework files and bundle settings ---
# -------------------------------------------------

IF(NOT MACOSX)
    SET(frameworkNameList '')
ELSE()
    # included frameworks
    LIST(APPEND frameworkNameList
         Accelerate AppKit AudioToolbox AudioUnit Carbon
         Cocoa CoreAudio CoreAudioKit CoreMIDI
         DiscRecording Foundation IOKit Metal
         QuartzCore Security WebKit
    )

    # bundle settings
    SET(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    SET(bundlePrefix "eu.tensi.thomas.FluidSynthPlugin")
ENDIF(NOT MACOSX)

# #################
# ### FUNCTIONS ###
# #################

FUNCTION(adaptLibraryFileLinkage)
    # copies library files in MacOSX to intermediate dynamic library
    # file directory and changes library paths in those libraries to a
    # relative path

    # copy files into intermediate directory
    FILE(MAKE_DIRECTORY ${dynamicLibrariesDirectory})
    SET(originalDynamicLibraryList ${dynamicLibraryNameList})
    LIST(TRANSFORM originalDynamicLibraryList
         PREPEND "${originalDynamicLibrariesDirectory}/")
    FILE(COPY ${originalDynamicLibraryList}
         DESTINATION ${dynamicLibrariesDirectory})

    # scan all files and change paths to "@loader_path"
    EXECUTE_PROCESS(COMMAND "bash"
                    ${dynamicLibraryAdaptationScript}
                    ${dynamicLibrariesDirectory})
ENDFUNCTION(adaptLibraryFileLinkage)

#--------------------

FUNCTION(addPluginNibFileForAUTarget targetName resourcePath)
    # adds a NIB file into resource folder <resourcePath> for target
    # <targetName> representing an audio unit library

    SET(targetNibFileName ${resourcePath}/${nibFileName})
    ADD_CUSTOM_COMMAND(TARGET ${targetName} POST_BUILD
                       COMMAND ibtool
                               --strip ${targetNibFileName}
                               --output-format human-readable-text
                               ${nibFileSourcePath})
ENDFUNCTION(addPluginNibFileForAUTarget)

#--------------------

FUNCTION(makePluginAUTarget pluginName libraryName targetName)
    # defines a static effect library as target <targetName> with
    # the engine classes for given effect named <pluginName>

    SET(plistFileName ${pluginName}_AppleInfo.plist)
    SET(principalClassName "NSApplication")

    ADD_LIBRARY(${targetName}
        MODULE ${src${pluginName}FileList_AU})

    addCompilerFlags(${targetName} TRUE)

    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC
                               ${additionalIncludeDirectoryList})

    TARGET_LINK_LIBRARIES(${targetName} JuceFramework)

    # add Apple frameworks to library list
    FOREACH(frameworkName ${frameworkNameList})
      TARGET_LINK_LIBRARIES(${targetName}
                            "-framework ${frameworkName}")
    ENDFOREACH(frameworkName)

    # make the specific PLIST file
    SET(JUCE_manufacturerName "DrTT")
    SET(JUCE_auMainType "aumu")
    SET(shortName "Flui")
    SET(JUCE_auSubType ${shortName})
    CONFIGURE_FILE(${plistFileTemplate} ${plistFileName})

    SET_TARGET_PROPERTIES(${targetName} PROPERTIES
        BUNDLE_EXTENSION component
        BUNDLE TRUE
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION component
        XCODE_ATTRIBUTE_LIBRARY_STYLE Bundle
        XCODE_ATTRIBUTE_GENERATE_PKGINFO_FILE YES
        MACOSX_BUNDLE_BUNDLE_NAME ${libraryName}
        MACOSX_BUNDLE_GUI_IDENTIFIER ${bundlePrefix}.${libraryName}
        MACOSX_BUNDLE_INFO_PLIST ${plistFileName}
        OUTPUT_NAME ${libraryName}
    )

    # add resources for audio unit library
    SET(resourcePath ${libraryName}.component/Contents/Resources)
    ADD_CUSTOM_COMMAND(TARGET ${targetName} POST_BUILD
                       COMMAND mkdir -p ${resourcePath})
    addPluginNibFileForAUTarget(${targetName} ${resourcePath})
ENDFUNCTION(makePluginAUTarget)

#--------------------

FUNCTION(makePluginStaticLibraryTarget pluginName targetName)
    # defines a static effect library as target <targetName> with
    # the engine classes for given effect named <pluginName>

    ADD_LIBRARY(${targetName} STATIC
                ${src${pluginName}FileListSTD}
                ${debuggerVisualizationFileName})

    addCompilerFlags(${targetName} TRUE)

    TARGET_INCLUDE_DIRECTORIES(${targetName}
                               PUBLIC ${src${pluginName}Directory})

    SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                          POSITION_INDEPENDENT_CODE TRUE
                          INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
ENDFUNCTION(makePluginStaticLibraryTarget)

#--------------------

FUNCTION(makePluginVSTTarget pluginName libraryName targetName)
    # defines a effect VST plugin as target <targetName> with
    # the VST wrapper classes combined with the static library for
    # given effect named <pluginName>

    IF(MACOSX)
        # on Apple platform a VST plugin is a library bundle
        ADD_LIBRARY(${targetName} MODULE
                    ${debuggerVisualizationFileName}
                    ${src${pluginName}FileList_VST})
    ELSE()
        # VST plugin is a dynamic library
        ADD_LIBRARY(${targetName} SHARED
                    ${debuggerVisualizationFileName}
                    ${src${pluginName}FileList_VST})
    ENDIF()

    addCompilerFlags(${targetName} TRUE)

    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC
                               ${additionalIncludeDirectoryList})

    TARGET_LINK_LIBRARIES(${targetName} JuceFramework)

    IF(MACOSX)
        # add framework libraries
        FOREACH(frameworkName ${frameworkNameList})
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
                              SUFFIX ${libraryNameExtension})

        IF(LINUX)
            SET(libraryFileDirectory ${buildDirectory})
        ELSE()
            SET(libraryFileDirectory
                ${buildDirectory}/${configurationSubdirectory})
        ENDIF()

        SET(libraryFileName
            ${libraryFileDirectory}/${libraryName}${libraryNameExtension})
        STRING(CONCAT vst3Directory
               "${buildDirectory}"
               "/${libraryName}.vst3"
               "/Contents/${vst3PlatformName}")
        FILE(MAKE_DIRECTORY ${vst3Directory})
        
        ADD_CUSTOM_COMMAND(TARGET ${targetName} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy
                                   ${libraryFileName}
                                   ${vst3Directory}
                                   COMMENT "Bundling ${libraryName}.")
    ENDIF()
ENDFUNCTION(makePluginVSTTarget)

#--------------------

FUNCTION(makeInternalDocumentationTarget targetName)
    # defines the target for the DoxyGen documentation as HTML files

    SET(doxygenRootFile ${doxygenTargetDirectory}/html/index.html)
    SET(DOXYGEN_SHOW_PRIVATE_FEATURES YES)
    CONFIGURE_FILE(${docDoxygenFileTemplate} ${docDoxygenFileName})

    ADD_CUSTOM_COMMAND(OUTPUT ${doxygenRootFile}
                       COMMAND ${DOXYGEN_EXECUTABLE} ${docDoxygenFileName}
                       WORKING_DIRECTORY ${buildDirectory}
                       COMMENT "Building DoxyGen API Documentation."
                       DEPENDS ${allSrcFileList})

    ADD_CUSTOM_TARGET(${targetName} DEPENDS ${doxygenRootFile})
ENDFUNCTION(makeInternalDocumentationTarget)

#--------------------

FUNCTION(makeJuceFrameworkTarget)
    # defines a JUCE static library as target <JuceFrameWork> with
    # classes from the JUCE modules directory

    SET(targetName JuceFramework)

    ADD_LIBRARY(${targetName} STATIC EXCLUDE_FROM_ALL
                ${srcJuceFacadeFileListSTD})

    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC
        ${additionalIncludeDirectoryList})

    # do not check the JUCE library functions at all
    addCompilerFlags(${targetName} FALSE)

    SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                          POSITION_INDEPENDENT_CODE TRUE
                          INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
ENDFUNCTION(makeJuceFrameworkTarget)

#--------------------

FUNCTION(makeLaTeXDocumentationTarget targetName)
    # defines the target for the LaTeX documentation as a PDF manual;
    # does a two phase compilation to get the TOC and AUX files
    # correct

    SET(commonLaTeXParameters
        -interaction=nonstopmode
        --output-directory=${buildDirectory})

    CMAKE_PATH(GET PDFLATEX_COMPILER PARENT_PATH teXFotCommand)
    SET(teXFotCommand ${teXFotCommand}/texfot)

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

    SET(docLaTeXFileNameStemBuildPath
        ${buildDirectory}/${docLaTeXFileNameStem})

    SET(docLaTeXPdfFileName ${docLaTeXFileNameStemBuildPath}.pdf)

    IF(WINDOWS)
        SET(nullDeviceName "NUL")
    ELSE()
        SET(nullDeviceName "/dev/null")
    ENDIF(WINDOWS)

    # make a temporary copy of documentation and figures into build
    # directory and then process files there via LaTeX compiler
    ADD_CUSTOM_COMMAND(OUTPUT  ${docLaTeXLocalFileName}
                       DEPENDS ${docLaTeXFileName}
                       COMMAND ${CMAKE_COMMAND} -E copy_directory
                               ${docLaTeXDirectory}/figures
                               ${buildDirectory}/figures
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different
                               ${docLaTeXFileName}
                               ${docLaTeXLocalFileName}
                       COMMENT "Copying LaTeX Files.")

    ADD_CUSTOM_COMMAND(OUTPUT  ${docLaTeXPdfFileName}
                       DEPENDS ${docLaTeXLocalFileName}
                       COMMAND ${pdfLaTeXCommandA}
                               ${docLaTeXFileNameStem}.ltx >${nullDeviceName}
                       COMMAND ${pdfLaTeXCommandB}
                               ${docLaTeXFileNameStem}.ltx
                       COMMENT "Generating Manual.")

    ADD_CUSTOM_TARGET(${targetName}
                      DEPENDS ${docLaTeXPdfFileName})
ENDFUNCTION(makeLaTeXDocumentationTarget)

###########
# TARGETS #
###########

INCLUDE_DIRECTORIES(${standardIncludeDirectoryList})

# ==================================
# === intermediate library files ===
# ==================================

# --- juce library with classes from JUCE ---
makeJuceFrameworkTarget()

ADD_CUSTOM_TARGET(SupportLibraries ALL)
ADD_DEPENDENCIES(SupportLibraries JuceFramework)

# ==============================
# === effective target files ===
# ==============================

ADD_CUSTOM_TARGET(FluidSynthPlugins ALL)

# -----------------------------------------------------------------
# --- adapt library files on MacOSX                             ---
# -----------------------------------------------------------------

IF(MACOSX)
    adaptLibraryFileLinkage()
ENDIF()

# -----------------------------------------------------------------
# --- build command-line fluidsynth file converter              ---
# -----------------------------------------------------------------

SET(targetName ${converterName})
LIST(APPEND fluidSynthConverterFileList
     ${commonSrcFileList}
     ${srcBaseModulesDirectory}/CommandLineArguments.cpp
     ${srcMidiSupportDirectory}/MidiFile.cpp
     ${srcAudioSupportDirectory}/WaveFile.cpp
     ${srcFluidSynthConverterDirectory}/FluidSynthFileConverter.cpp
     ${srcFluidSynthConverterDirectory}/MainStub.cpp
     ${debuggerVisualizationFileName}
)

ADD_EXECUTABLE(${targetName} ${fluidSynthConverterFileList})
addCompilerFlags(${targetName} TRUE)
ADD_DEPENDENCIES(FluidSynthPlugins ${targetName})

# -----------------------------------------------------------------
# --- build a dynamic library for each effect with Juce GUI and ---
# --- VST client plus that plugin                               ---
# -----------------------------------------------------------------

FOREACH(pluginName ${pluginNameList})
    SET(libraryName ${pluginName})
    SET(targetName ${libraryName}_VST)
    makePluginVSTTarget(${pluginName} ${libraryName} ${targetName})
    ADD_DEPENDENCIES(FluidSynthPlugins ${targetName})
ENDFOREACH(pluginName)

# -----------------------------------------------------------------
# --- build an audio unit app for each effect with Juce GUI and ---
# --- AUv3 client plus the static library of that plugin        ---
# -----------------------------------------------------------------

IF(MACOSX)
    FOREACH(pluginName ${pluginNameList})
        SET(libraryName ${pluginName})
        SET(targetName ${libraryName}_AU)
        makePluginAUTarget(${pluginName} ${libraryName} ${targetName})
        ADD_DEPENDENCIES(FluidSynthPlugins ${targetName})
    ENDFOREACH(pluginName)
ENDIF()

# ---------------------------
# --- LaTeX Documentation ---
# ---------------------------

ADD_CUSTOM_TARGET(documentation ALL)
 
IF(NOT LATEX_PDFLATEX_FOUND)
    MESSAGE(STATUS "No PDFLaTeX compiler found --> skipping.")
ELSE()
    SET(targetName pdfDocumentation)
    makeLaTeXDocumentationTarget(${targetName})
    ADD_DEPENDENCIES(documentation ${targetName})
ENDIF()

# -----------------------------
# --- DoxyGen Documentation ---
# -----------------------------

IF(NOT DOXYGEN_FOUND)
    MESSAGE(STATUS "No DoxyGen or GraphViz found --> skipping.")
ELSE()
    SET(targetName internalDocumentation)
    makeInternalDocumentationTarget(${targetName})
    ADD_DEPENDENCIES(documentation ${targetName})
ENDIF()

# ====================
# === INSTALLATION ===
# ====================

SET(platformTargetSubdirectory
    ${platformTargetDirectory}/${platformName}/${configurationSubdirectory})
MESSAGE(STATUS "platformTargetSubdirectory = "
        ${platformTargetSubdirectory})
FILE(MAKE_DIRECTORY ${platformTargetSubdirectory})

IF(MSVC)
    # make installation part of build in MSVC
    SET(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
ENDIF()

# --- install the documentation ---
IF(LATEX_PDFLATEX_FOUND)
    # WORKAROUND: find the path of the PDF file from some target
    SET(specificTargetDirectory ${targetDirectory}/doc)
    FILE(REMOVE_RECURSE ${specificTargetDirectory})
    INSTALL(FILES ${buildDirectory}/${docLaTeXFileNameStem}.pdf
            DESTINATION ${specificTargetDirectory})
ENDIF()

# --- install the regression test files ---
SET(specificTargetDirectory ${targetDirectory}/test)
FILE(REMOVE_RECURSE ${specificTargetDirectory})
INSTALL(FILES ${regressionTestFileList}
        DESTINATION ${specificTargetDirectory})

# --- install the Fluidsynth File Converter to platform directory ---
SET(targetConverterDirectory
    ${platformTargetSubdirectory}/${converterName})
FILE(REMOVE_RECURSE ${targetConverterDirectory})
INSTALL(TARGETS ${converterName}
        DESTINATION ${targetConverterDirectory})
INSTALL(FILES ${dynamicLibraryList}
        DESTINATION ${targetConverterDirectory})

# --- install the VST3i plugin to platform directory ---
FOREACH(pluginName ${pluginNameList})
    SET(libraryName ${pluginName})
    SET(targetVST3Directory
        ${platformTargetSubdirectory}/VST3)
    FILE(REMOVE_RECURSE ${targetVST3Directory})

    IF(NOT MACOSX)
        SET(targetVST3Path ${targetVST3Directory}/${libraryName}.vst3)
        INSTALL(DIRECTORY ${buildDirectory}/${libraryName}.vst3/Contents
                DESTINATION ${targetVST3Path})
        INSTALL(FILES ${dynamicLibraryList}
                DESTINATION ${targetVST3Path}/Contents/${vst3PlatformName})
    ELSE()
        SET(targetName ${libraryName}_VST)
        INSTALL(TARGETS ${targetName}
                DESTINATION ${targetVST3Directory})
        INSTALL(FILES ${dynamicLibraryList}
                DESTINATION ${targetVST3Directory}/${libraryName}.vst3/Contents/MacOS)
    ENDIF()
ENDFOREACH(pluginName)

# --- install the AU plugins to platform directory ---
IF(MACOSX)
    FOREACH(pluginName ${pluginNameList})
        SET(targetAUDirectory
            ${platformTargetSubdirectory}/AU)
        FILE(REMOVE_RECURSE ${targetAUDirectory})
        SET(libraryName ${pluginName})
        SET(targetName ${libraryName}_AU)
        INSTALL(TARGETS ${targetName} DESTINATION ${targetAUDirectory})
        INSTALL(FILES ${dynamicLibraryList}
                DESTINATION ${targetAUDirectory}/${libraryName}.component/Contents/MacOS)
      ENDFOREACH(pluginName)
ENDIF(MACOSX)
