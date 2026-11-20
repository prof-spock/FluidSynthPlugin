# -*- coding: utf-8 -*-
#
# Global Constants and functions in CMAKE for DrTT plugins


# #################
# ### CONSTANTS ###
# #################

IF(WINDOWS)
    SET(GLOBP_juceFileExtension cpp)
    SET(GLOBP_vst3LibraryNameExtension ".vst3")
    SET(GLOBP_vst3PlatformName "x86_64-win")
ELSEIF(LINUX)
    SET(GLOBP_juceFileExtension cpp)
    SET(GLOBP_vst3LibraryNameExtension ".so")
    SET(GLOBP_vst3PlatformName "x86_64-linux")
ELSE()
    SET(GLOBP_juceFileExtension mm)
    SET(GLOBP_vst3LibraryNameExtension ".so")
    SET(GLOBP_vst3PlatformName "MacOS")
ENDIF()

# -----------------------------
# --- Apple framework files ---
# -----------------------------

IF(NOT MACOS)
    SET(GLOBP_frameworkNameList )
ELSE()
    # included frameworks
    LIST(APPEND GLOBP_frameworkNameList
         Accelerate AppKit AudioToolbox AudioUnit Carbon
         Cocoa CoreAudio CoreAudioKit CoreMIDI
         DiscRecording Foundation IOKit Metal MetalKit
         QuartzCore Security WebKit
    )
ENDIF()

# #################
# ### FUNCTIONS ###
# #################

#=========
#= BUILD =
#=========

FUNCTION(GLOBP_addPluginNibFileForAUTarget targetName resourcePath)
    # adds a NIB file into resource folder <resourcePath> for target
    # <targetName> representing an audio unit library

    CMAKE_PATH(SET targetNibFileName NORMALIZE
               ${resourcePath}/${GLOB_nibFileName})
    ADD_CUSTOM_COMMAND(TARGET ${targetName} POST_BUILD
                       COMMAND ibtool
                               --strip ${targetNibFileName}
                               --output-format human-readable-text
                               ${GLOB_nibFileSourcePath})
ENDFUNCTION(GLOBP_addPluginNibFileForAUTarget)

#--------------------

MACRO(GLOBP_appendJuceIncludeDirectories
      resultListVariableName juceDirectory)
    # appends list of JUCE include directories rooted at
    # <juceModulesDirectory> to list variable named
    # <resultListVariableName>

    CMAKE_PATH(SET juceModulesDirectory NORMALIZE
               ${juceDirectory}/modules)

    SET(audioProcessorsSubdirectory juce_audio_processors_headless)
    SET(juceFormatTypesDirectory
        ${juceModulesDirectory}/${audioProcessorsSubdirectory}/format_types)
    SET(lv2JuceFormatTypesDirectory
        ${juceFormatTypesDirectory}/LV2_SDK)
    SET(vst3JuceFormatTypesDirectory
        ${juceFormatTypesDirectory}/VST3_SDK)

    LIST(APPEND ${resultListVariableName}
         ${juceModulesDirectory}
         ${juceModulesDirectory}/juce_audio_plugin_client/AU
         ${vst3JuceFormatTypesDirectory}
         ${lv2JuceFormatTypesDirectory}
         ${lv2JuceFormatTypesDirectory}/lv2
         ${lv2JuceFormatTypesDirectory}/serd
         ${lv2JuceFormatTypesDirectory}/sord
         ${lv2JuceFormatTypesDirectory}/sord/src
         ${lv2JuceFormatTypesDirectory}/sratom
         ${lv2JuceFormatTypesDirectory}/lilv
         ${lv2JuceFormatTypesDirectory}/lilv/src
    )
ENDMACRO(GLOBP_appendJuceIncludeDirectories)

#--------------------

MACRO(GLOBP_appendJucePluginClientFiles
      resultListVariableName libraryFacadeDirectory)
    # appends list of JUCE plugin client files in the
    # <libraryFacadeDirectory> to list variable named
    # <resultListVariableName>

    SET(clientNameStemList
        aax aax_utils ara lv2 standalone unity vst2 vst3)
    SET(prefix
        "${libraryFacadeDirectory}/include_juce_audio_plugin_client_")

    UTIL_List_appendOtherTransformed(${resultListVariableName}
                                     clientNameStemList
                                     "${prefix}"
                                     ".${GLOBP_juceFileExtension}")
ENDMACRO(GLOBP_appendJucePluginClientFiles)

#--------------------

FUNCTION(GLOBP_makeTarget_juceFramework
         jucePrefix)
    # makes a JUCE static library target named <JuceFramework> with
    # classes from the JUCE modules directory

    # --- the file name list of facade files for JUCE; those files ---
    # --- reference real implementations in JUCE; note that on     ---
    # --- Apple platforms the Objective-C++ (.mm) files are used   ---
    # --- instead of the standard C++ (.cpp) files                 ---
    SET(srcJuceFacadeFileList )

    SET(juceLibraryNameStemList
        audio_basics audio_devices audio_formats audio_processors
        audio_processors_headless audio_utils core data_structures
        events graphics gui_basics gui_basics_2 gui_basics_3
        gui_basics_4 gui_basics_5 gui_extra
    )

    UTIL_List_appendOtherTransformed(srcJuceFacadeFileList
                                     juceLibraryNameStemList
                                     "${jucePrefix}_"
                                     ".${GLOBP_juceFileExtension}")

    IF(JUCE_VERSION GREATER_EQUAL 9)
        SET(juceLibraryNameStemList
            core_zlib graphics_libjpg_1 graphics_libjpg_2
            graphics_libjpg_3 graphics_libpng graphics_libwebp_dec
            graphics_libwebp_dsp_dec_avx2 graphics_libwebp_dsp_dec
            graphics_libwebp_dsp_dec_sse41
            graphics_libwebp_dsp_enc_avx2 graphics_libwebp_dsp_enc
            graphics_libwebp_dsp_enc_sse41 graphics_libwebp_enc
            graphics_libwebp_sharpyuv graphics_libwebp_utils
            graphics_lunasvg
        )

        UTIL_List_appendOtherTransformed(srcJuceFacadeFileList
                                         juceLibraryNameStemList
                                         "${jucePrefix}_"
                                         ".c")
    ENDIF()

    IF(JUCE_VERSION GREATER_EQUAL 8)
        LIST(APPEND srcJuceFacadeFileList
             ${jucePrefix}_core_compilationtime.cpp
             ${jucePrefix}_graphics_harfbuzz.cpp
             ${jucePrefix}_graphics_sheenbidi.c)
    ENDIF()

    IF(MACOS)
        SET(juceAusdkLibraryNameStemList
            base buffer bufferallocator componentbase effectbase
            inputelement midibase midieffectbase musicdevicebase
            outputelement plugindispatch scopeelement
        )

        UTIL_List_appendOtherTransformed(srcJuceFacadeFileList
                                         juceAusdkLibraryNameStemList
                                         "${jucePrefix}_audio_ausdk_"
                                         ".cpp")            
    ENDIF()

    SET(targetName JuceFramework)

    ADD_LIBRARY(${targetName} STATIC EXCLUDE_FROM_ALL
                ${srcJuceFacadeFileList})

    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC
                               ${additionalIncludeDirectoryList})

    # do not check the JUCE library functions at all
    CMPCONF_addCompilerFlags(${targetName} FALSE)

    SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                          POSITION_INDEPENDENT_CODE TRUE
                          INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
ENDFUNCTION(GLOBP_makeTarget_juceFramework)

#--------------------

FUNCTION(GLOBP_makeTarget_pluginAU
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

    IF(MACOS)
        SET(plistFileName ${pluginName}_AppleInfo.plist)
        SET(principalClassName "NSApplication")

        ADD_LIBRARY(${targetName} MODULE ${sourceFileList})

        CMPCONF_addCompilerFlags(${targetName} TRUE)

        TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC
                                   ${includeDirectoryList})

        TARGET_LINK_LIBRARIES(${targetName}
                              ${libraryList})

        # add Apple frameworks to library list
        FOREACH(frameworkName ${GLOBP_frameworkNameList})
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
        GLOBP_addPluginNibFileForAUTarget(${targetName} ${resourcePath})
    ENDIF()
ENDFUNCTION(GLOBP_makeTarget_pluginAU)

#--------------------

FUNCTION(GLOBP_makeTarget_pluginStaticLib
         targetName pluginDirectory fileList)
    # makes a static library target named <targetName> with the engine
    # classes for given plugin named <pluginName>

    ADD_LIBRARY(${targetName} STATIC ${fileList})

    CMPCONF_addCompilerFlags(${targetName} TRUE)

    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC ${pluginDirectory})

    SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                          POSITION_INDEPENDENT_CODE TRUE
                          INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
ENDFUNCTION(GLOBP_makeTarget_pluginStaticLib)

#--------------------

FUNCTION(GLOBP_makeTarget_pluginVST
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

    IF(MACOS)
        # on Apple platform a VST plugin is a library bundle
        ADD_LIBRARY(${targetName} MODULE
                    ${debuggerVisualizationFileNameList}
                    ${sourceFileList})
    ELSE()
        # VST plugin is a dynamic library
        ADD_LIBRARY(${targetName} SHARED
                    ${debuggerVisualizationFileNameList}
                    ${sourceFileList})
    ENDIF()

    CMPCONF_addCompilerFlags(${targetName} TRUE)

    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC ${includeDirectoryList})
    TARGET_LINK_LIBRARIES(${targetName} ${libraryList})

    IF(MACOS)
        # add framework libraries
        FOREACH(frameworkName ${GLOBP_frameworkNameList})
            TARGET_LINK_LIBRARIES(${targetName}
                                  "-framework ${frameworkName}")
        ENDFOREACH(frameworkName)
    ENDIF()

    IF(LINUX)
        TARGET_LINK_LIBRARIES(${targetName} ${unixLibraryList})
        TARGET_LINK_LIBRARIES(${targetName} dl pthread rt)
    ENDIF()

    # put library into a bundle structure regardless of operating
    # system
    IF(MACOS)
        SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                              BUNDLE TRUE
                              BUNDLE_EXTENSION vst3
                              OUTPUT_NAME ${libraryName}
                              SUFFIX "")
    ELSE()
        SET_TARGET_PROPERTIES(${targetName} PROPERTIES
                              OUTPUT_NAME ${libraryName}
                              PREFIX ""
                              SUFFIX ${GLOBP_vst3LibraryNameExtension})

        IF(LINUX)
            CMAKE_PATH(SET libraryFileDirectory NORMALIZE
                       ${GLOB_buildDirectory})
        ELSE()
            CMAKE_PATH(SET libraryFileDirectory NORMALIZE
                       ${GLOB_buildDirectory}/${GLOB_configurationSubdirectory})
        ENDIF()

        CMAKE_PATH(SET libraryFileName NORMALIZE
                   ${libraryFileDirectory}/${libraryName}${GLOBP_vst3LibraryNameExtension})

        SET(vst3Directory )
        CMAKE_PATH(APPEND vst3Directory
                   "${GLOB_buildDirectory}"
                   "${libraryName}.vst3"
                   "Contents"
                   "${GLOBP_vst3PlatformName}")
        
        ADD_CUSTOM_COMMAND(TARGET ${targetName} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory
                                   ${vst3Directory}
                           COMMAND ${CMAKE_COMMAND} -E copy
                                   ${libraryFileName}
                                   ${vst3Directory}
                                   COMMENT "Bundling ${libraryName}.")
    ENDIF()
ENDFUNCTION(GLOBP_makeTarget_pluginVST)

#================
#= INSTALLATION =
#================

FUNCTION(GLOBP_install_pluginAU libraryName dynamicLibraryList)
    # install AU plugin given by <libraryName> together with dynamic
    # libraries in <dynamicLibraryList>

    IF(MACOS)
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
    ENDIF()
ENDFUNCTION(GLOBP_install_pluginAU)

#--------------------

FUNCTION(GLOBP_install_pluginVST libraryName dynamicLibraryList)
    # install VST plugin given by <libraryName> together with dynamic
    # libraries in <dynamicLibraryList>

    CMAKE_PATH(SET targetVST3Directory NORMALIZE
               ${GLOB_platformTargetSubdirectory}/VST3)
    FILE(REMOVE_RECURSE ${targetVST3Directory})

    IF(NOT MACOS)
        CMAKE_PATH(SET targetVST3Path NORMALIZE
                   ${targetVST3Directory}/${libraryName}.vst3)
        INSTALL(DIRECTORY ${GLOB_buildDirectory}/${libraryName}.vst3/Contents
                DESTINATION ${targetVST3Path})
        INSTALL(FILES ${dynamicLibraryList}
                DESTINATION ${targetVST3Path}/Contents/${GLOBP_vst3PlatformName})
    ELSE()
        SET(targetName ${libraryName}_VST)
        INSTALL(TARGETS ${targetName}
                DESTINATION ${targetVST3Directory})
        INSTALL(FILES ${dynamicLibraryList}
                DESTINATION ${targetVST3Directory}/${libraryName}.vst3/Contents/MacOS)
    ENDIF()
ENDFUNCTION(GLOBP_install_pluginVST)
