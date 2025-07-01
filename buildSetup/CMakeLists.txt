###############################################
# CMAKE file for the FluidSynthPlugin project #
#                                             #
# Dr. Thomas Tensi, 2022-04                   #
###############################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(FluidSynthPlugin
        VERSION 1.0
        LANGUAGES C CXX)

# =============================
# === program configuration ===
# =============================

INCLUDE(GlobalSettings.cmake)
INCLUDE(GlobalSettings_Plugin.cmake)

# --- compiler ---
INCLUDE(PluginSpecificCompilerSettings.cmake)
# first JUCE plugin settings, then global compiler settings
INCLUDE(CPPCompilerConfiguration_Plugin.cmake)
INCLUDE(CPPCompilerConfiguration.cmake)

# --- LaTeX ---
FIND_PACKAGE(LATEX QUIET OPTIONAL_COMPONENTS PDFLATEX)

# --- DoxyGen ---
FIND_PACKAGE(Doxygen QUIET)

# --- JUCE ---
FIND_PACKAGE(juce QUIET)

# local configuration settings
INCLUDE(LocalConfiguration.cmake)

# ===========================

# the name stems of all plugins to produce (currently only a single
# one)
SET(pluginNameList FluidSynthPlugin)

# the name of the file converter
SET(converterName FluidSynthFileConverter)

# ===================
# === directories ===
# ===================

# --- source directories ---
CMAKE_PATH(SET srcDirectory NORMALIZE
           ${CMAKE_CURRENT_SOURCE_DIR}/../src)
CMAKE_PATH(SET srcAudioSupportDirectory NORMALIZE
           ${srcDirectory}/Audio)
CMAKE_PATH(SET srcBaseModulesDirectory NORMALIZE
           ${srcDirectory}/BaseModules)
CMAKE_PATH(SET srcBaseTypesDirectory NORMALIZE
           ${srcDirectory}/BaseTypes)
CMAKE_PATH(SET srcContainersDirectory NORMALIZE
           ${srcBaseTypesDirectory}/Containers)
CMAKE_PATH(SET srcGenericTypesDirectory NORMALIZE
           ${srcBaseTypesDirectory}/GenericTypes)
CMAKE_PATH(SET srcPrimitivesDirectory NORMALIZE
           ${srcBaseTypesDirectory}/Primitives)
CMAKE_PATH(SET srcLibrariesDirectory NORMALIZE
           ${srcDirectory}/Libraries)
CMAKE_PATH(SET srcFluidSynthLibDirectory NORMALIZE
           ${srcDirectory}/Libraries/FluidSynth)
CMAKE_PATH(SET srcJuceFacadeDirectory NORMALIZE
           ${srcDirectory}/Libraries/JuceFacade)
CMAKE_PATH(SET srcMidiSupportDirectory NORMALIZE
           ${srcDirectory}/MIDI)
CMAKE_PATH(SET srcMainRootDirectory NORMALIZE
           ${srcDirectory}/Main)

# directories for the plugins (currently only a single one) and the
# player
CMAKE_PATH(SET srcFluidSynthPluginDirectory NORMALIZE
           ${srcMainRootDirectory}/FluidSynthPlugin)
CMAKE_PATH(SET srcFluidSynthConverterDirectory NORMALIZE
           ${srcMainRootDirectory}/${converterName})

# the original location of the dynamic libraries
CMAKE_PATH(SET originalDynamicLibrariesDirectory NORMALIZE
           ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/${GLOB_platformName})

IF(NOT MACOS)
    # no transformation needed: all libraries stay in their original
    # location
    CMAKE_PATH(SET effectiveDynamicLibrariesDirectory NORMALIZE
               ${originalDynamicLibrariesDirectory})
ELSE()
    # libraries must be transformed (by throwing away the absolute
    # path names of referenced libraries); define intermediate library
    # directory
    CMAKE_PATH(SET effectiveDynamicLibrariesDirectory NORMALIZE
               ${GLOB_buildDirectory}/libraries)
ENDIF()

CMAKE_PATH(SET testFileDirectory NORMALIZE
           ${CMAKE_CURRENT_SOURCE_DIR}/../test)

# --- include directories ---

CMAKE_PATH(SET juceModulesDirectory NORMALIZE
           ${JUCE_DIR}/modules)

SET(standardIncludeDirectoryList
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

SET(additionalIncludeDirectoryList
    ${srcJuceFacadeDirectory}
    ${juceModulesDirectory}
    ${juceModulesDirectory}/juce_audio_plugin_client/AU
    ${juceModulesDirectory}/juce_audio_processors/format_types/VST3_SDK
)

# --- add dynamic linkage library for MacOS and Unix ---
IF(NOT WINDOWS)
    LINK_LIBRARIES(${CMAKE_DL_LIBS})
ENDIF(NOT WINDOWS)

# --- add include directories and libraries for Unix ---
IF(LINUX)
    FIND_PACKAGE(PkgConfig REQUIRED) 
    SET(relevantPackageList freetype2)
    PKG_CHECK_MODULES(linuxPackages ${relevantPackageList})
    SET(unixIncludeDirectoryList /usr/include/freetype2)
    SET(unixLibraryList ${linuxPackages_LINK_LIBRARIES})

    LIST(APPEND additionalIncludeDirectoryList
         ${unixIncludeDirectoryList}
    )
ENDIF(LINUX)

# ====================
# === source files ===
# ====================

SET(srcAudioModulesFileList
    ${srcAudioSupportDirectory}/AudioSampleFifoQueue.cpp
    ${srcAudioSupportDirectory}/AudioSampleFifoQueueVector.cpp
    ${srcAudioSupportDirectory}/AudioSampleList.cpp
    ${srcAudioSupportDirectory}/AudioSampleListVector.cpp
)

SET(srcBaseModulesFileList
    ${srcBaseModulesDirectory}/Environment.cpp
    ${srcBaseModulesDirectory}/File.cpp
    ${srcBaseModulesDirectory}/LoggingSupport.cpp
    ${srcBaseModulesDirectory}/OperatingSystem.cpp
    ${srcBaseModulesDirectory}/StringUtil.cpp
)

SET(srcContainersFileList
    ${srcContainersDirectory}/ByteList.cpp
    ${srcContainersDirectory}/Dictionary.cpp
    ${srcContainersDirectory}/NaturalList.cpp
    ${srcContainersDirectory}/StringList.cpp
)

SET(srcPrimitivesFileList
    ${srcPrimitivesDirectory}/MyString.cpp
    ${srcPrimitivesDirectory}/Real.cpp
    ${srcPrimitivesDirectory}/TimeAndDuration.cpp
)

SET(srcFluidSynthMidiSupportFileList
    ${srcMidiSupportDirectory}/MidiEvent.cpp
    ${srcMidiSupportDirectory}/MidiEventList.cpp
    ${srcMidiSupportDirectory}/MidiEventConverter.cpp
    ${srcMidiSupportDirectory}/MidiPresetIdentification.cpp
)

SET(srcFluidSynthLibraryFileList
    ${srcLibrariesDirectory}/DynamicLibrary.cpp
    ${srcFluidSynthLibDirectory}/FluidSynth.cpp
    ${srcFluidSynthLibDirectory}/FluidSynthSettings.cpp
    ${srcFluidSynthLibDirectory}/FluidSynthSoundFont.cpp
    ${srcFluidSynthLibDirectory}/FluidSynthSynthesizer.cpp
)

SET(commonSrcFileList
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
    CMAKE_PATH(SET effectDirectory NORMALIZE
               ${src${pluginName}Directory})

    SET(fileListName src${pluginName}FileList_VST)

    SET(${fileListName}
        ${srcMainRootDirectory}/BuildInformation.cpp
        ${effectDirectory}/${pluginName}_Editor.cpp
        ${effectDirectory}/${pluginName}_EditorSupport.cpp
        ${effectDirectory}/${pluginName}_EventProcessor.cpp
        ${effectDirectory}/JucePlugin-VST_1.${GLOBP_juceFileExtension}
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

IF(MACOS)
    FOREACH(pluginName ${pluginNameList})
        CMAKE_PATH(SET effectDirectory NORMALIZE
                   ${src${pluginName}Directory})

        SET(fileListName src${pluginName}FileList_AU)

        SET(${fileListName}
            ${srcMainRootDirectory}/BuildInformation.cpp
            ${effectDirectory}/${pluginName}_EventProcessor.cpp
            ${effectDirectory}/${pluginName}_Editor.cpp
            ${effectDirectory}/${pluginName}_EditorSupport.cpp
            ${effectDirectory}/JucePlugin-util.cpp
            ${effectDirectory}/JucePlugin-AU_1.mm
            ${effectDirectory}/JucePlugin-AU_2.mm
            ${effectDirectory}/JucePlugin-Standalone.cpp
        )

        LIST(APPEND allSrcFileList ${${fileListName}})
    ENDFOREACH(pluginName)
ENDIF(MACOS)

# -------------------------------------------------------------------
# --- the list of dynamic libraries per platform                  ---
# -------------------------------------------------------------------

IF(WINDOWS)
    SET(dynamicLibraryNameList
        libfluidsynth-3.dll libgcc_s_sjlj-1.dll libglib-2.0-0.dll
        libgobject-2.0-0.dll libgomp-1.dll libgthread-2.0-0.dll
        libinstpatch-2.dll libintl-8.dll sdl3.dll sndfile.dll
        libstdc++-6.dll libwinpthread-1.dll
    )
ELSEIF(MACOS)
    SET(dynamicLibraryNameList
        libFLAC.12.dylib libfluidsynth.3.dylib libglib-2.0.0.dylib
        libgthread-2.0.0.dylib libiconv.2.dylib libintl.8.dylib
        libmp3lame.0.dylib libmpg123.0.dylib libncurses.6.dylib
        libogg.0.dylib libopus.0.dylib libpcre2-8.0.dylib
        libreadline.8.dylib libSDL2-2.0.0.dylib libsndfile.1.dylib
        libvorbis.0.dylib libvorbisenc.2.dylib
    )
ELSEIF(LINUX)
    # no dynamic libraries used => for Linux FluidSynth must be
    # installed
    SET(dynamicLibraryNameList )
ENDIF()
 
SET(dynamicLibraryList ${dynamicLibraryNameList})
LIST(TRANSFORM dynamicLibraryList
     PREPEND ${effectiveDynamicLibrariesDirectory}/)

# --- regression test sources ---
SET(testFileName makeTestFiles.${GLOB_batchFileExtension})

IF(WINDOWS)
    SET(regressionTestFileList
        ${testFileDirectory}/${testFileName}
    )
ELSE()
    CONFIGURE_FILE(${testFileDirectory}/makeTestFiles.sh.in
                   ${GLOB_buildDirectory}/${testFileName}
                   @ONLY)
    SET(regressionTestFileList
        ${GLOB_buildDirectory}/${testFileName}
    )
ENDIF()

LIST(APPEND regressionTestFileList
     ${testFileDirectory}/test_bass.mid
     ${testFileDirectory}/test_drums.mid
     ${testFileDirectory}/test_keyboard.mid
     ${testFileDirectory}/SimpleTestSoundfont.sf2
     ${testFileDirectory}/testFluidSynth-44100-VST3.rpp
     ${testFileDirectory}/testFluidSynth-48000-VST3.rpp
     ${testFileDirectory}/testFluidSynth-48000-singleVoice-VST3.rpp
)

IF(MACOS)
    LIST(APPEND regressionTestFileList
         ${testFileDirectory}/testFluidSynth-48000-singleVoice-AU.rpp
    )
ENDIF()

###########
# TARGETS #
###########

INCLUDE_DIRECTORIES(${standardIncludeDirectoryList})

# ==================================
# === intermediate library files ===
# ==================================

# --- common services library (primitives containers and common audio) ---
GLOB_makeTarget_commonProjectLibrary("${commonSrcFileList}")

# --- additional JUCE settings ---
CMAKE_PATH(SET jucePrefix NORMALIZE
           ${srcJuceFacadeDirectory}/include_juce)

# --- juce library with classes from JUCE ---
GLOBP_makeTarget_juceFramework(${jucePrefix})

ADD_CUSTOM_TARGET(SupportLibraries ALL)
ADD_DEPENDENCIES(SupportLibraries JuceFramework)

# ==============================
# === effective target files ===
# ==============================

ADD_CUSTOM_TARGET(FluidSynthPlugins ALL)

# -----------------------------------------------------------------
# --- adapt library files on MacOS                              ---
# -----------------------------------------------------------------

IF(MACOS)
    GLOB_adaptLibraryFileLinkage(${originalDynamicLibrariesDirectory}
                                 ${effectiveDynamicLibrariesDirectory}
                                 "${dynamicLibraryNameList}"
    )
ENDIF()

# -----------------------------------------------------------------
# --- build command-line fluidsynth file converter              ---
# -----------------------------------------------------------------

SET(targetName ${converterName})
SET(fluidSynthConverterFileList
    ${srcBaseModulesDirectory}/CommandLineArguments.cpp
    ${srcMidiSupportDirectory}/MidiFile.cpp
    ${srcAudioSupportDirectory}/WaveFile.cpp
    ${srcFluidSynthConverterDirectory}/FluidSynthFileConverter.cpp
    ${srcFluidSynthConverterDirectory}/MainStub.cpp
    ${GLOB_debuggerVisualizationFileName}
)

ADD_EXECUTABLE(${targetName} ${fluidSynthConverterFileList})
CMPCONF_addCompilerFlags(${targetName} TRUE)
TARGET_LINK_LIBRARIES(${targetName} CommonProjectLibrary)
ADD_DEPENDENCIES(FluidSynthPlugins ${targetName})

# -----------------------------------------------------------------
# --- build a dynamic library for each effect with Juce GUI and ---
# --- VST client plus that plugin                               ---
# -----------------------------------------------------------------

FOREACH(pluginName ${pluginNameList})
    SET(libraryName ${pluginName})
    SET(targetName ${libraryName}_VST)
    SET(sourceFileList ${src${pluginName}FileList_VST})
    SET(libraryList CommonProjectLibrary JuceFramework)

    GLOBP_makeTarget_pluginVST(
        ${targetName} ${pluginName} ${libraryName}
        "${sourceFileList}"
        "${additionalIncludeDirectoryList}"
        "${libraryList}"
        "${unixLibraryList}"
    )

    ADD_DEPENDENCIES(FluidSynthPlugins ${targetName})
ENDFOREACH(pluginName)

# -----------------------------------------------------------------
# --- build an audio unit app for each effect with Juce GUI and ---
# --- AUv3 client plus the static library of that plugin        ---
# -----------------------------------------------------------------

IF(MACOS)
    FOREACH(pluginName ${pluginNameList})
        SET(libraryName ${pluginName})
        SET(targetName ${libraryName}_AU)
        SET(sourceFileList ${src${pluginName}FileList_AU})
        SET(libraryList CommonProjectLibrary JuceFramework)

        GLOBP_makeTarget_pluginAU(${targetName} ${pluginName} ${libraryName}
                                  ${pluginName} "DrTT: ${pluginName}"
                                  "aumu" "Flui"
                                  "${sourceFileList}"
                                  "${additionalIncludeDirectoryList}"
                                  "${libraryList}"
        )

        ADD_DEPENDENCIES(FluidSynthPlugins ${targetName})
    ENDFOREACH(pluginName)
ENDIF()

# ---------------------------
# --- documentation ---
# ---------------------------

GLOB_makeTarget_documentation("documentation" "${allSrcFileList}")

# ====================
# === INSTALLATION ===
# ====================

IF(CMPCONF_compilerIsMSVC)
    # make installation part of build in MSVC
    SET(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
ENDIF()

# --- install the documentation ---
GLOB_install_documentationPDF()

# --- install the regression test files ---
GLOB_install_testFiles("${regressionTestFileList}")

# --- install the Fluidsynth File Converter to platform directory ---
CMAKE_PATH(SET targetConverterDirectory NORMALIZE
           ${GLOB_platformTargetSubdirectory}/${converterName})
FILE(REMOVE_RECURSE ${targetConverterDirectory})
INSTALL(TARGETS ${converterName}
        DESTINATION ${targetConverterDirectory})
INSTALL(FILES ${dynamicLibraryList}
        DESTINATION ${targetConverterDirectory})

# --- install the VST3i and AU plugins to platform directory ---
FOREACH(pluginName ${pluginNameList})
    SET(libraryName ${pluginName})
    GLOBP_install_pluginVST(${libraryName} "${dynamicLibraryList}")
    GLOBP_install_pluginAU(${libraryName} "${dynamicLibraryList}")
ENDFOREACH(pluginName)
