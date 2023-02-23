# -*- coding: utf-8 -*-
#
# Local Settings for the C++-Compiler in CMAKE for the
# SoXPlugins
#

# --- list of defines for compiler ---

# manufacturer settings
SET(manufacturerAndSuiteDefineClauseList
    JucePlugin_Manufacturer=§DrTT§
    JucePlugin_ManufacturerEmail=§§
    JucePlugin_ManufacturerWebsite=§https://github.com/prof-spock/FluidSynthPlugin§
    JucePlugin_ManufacturerCode=0x44725454
    JucePlugin_AAXManufacturerCode=JucePlugin_ManufacturerCode)

# plugin suite settings
SET(manufacturerAndSuiteDefineClauseList
    ${manufacturerAndSuiteDefineClauseList}
    JucePlugin_Version=1.0.0
    JucePlugin_VersionCode=0x10000
    JucePlugin_VersionString=§1.0.0§)

SET(cppDefineClauseList
    _LIB
    JUCE_APP_VERSION=1.0.0
    JUCE_APP_VERSION_HEX=0x10000
    JUCE_DISPLAY_SPLASH_SCREEN=1
    JUCE_USE_DARK_SPLASH_SCREEN=0
    JUCE_DONT_DECLARE_PROJECTINFO=1
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    JUCE_MODULE_AVAILABLE_juce_audio_basics=1
    JUCE_MODULE_AVAILABLE_juce_audio_devices=1
    JUCE_MODULE_AVAILABLE_juce_audio_formats=1
    JUCE_MODULE_AVAILABLE_juce_audio_plugin_client=1
    JUCE_MODULE_AVAILABLE_juce_audio_processors=1
    JUCE_MODULE_AVAILABLE_juce_audio_utils=1
    JUCE_MODULE_AVAILABLE_juce_core=1
    JUCE_MODULE_AVAILABLE_juce_data_structures=1
    JUCE_MODULE_AVAILABLE_juce_events=1
    JUCE_MODULE_AVAILABLE_juce_graphics=1
    JUCE_MODULE_AVAILABLE_juce_gui_basics=1
    JUCE_MODULE_AVAILABLE_juce_gui_extra=1
    JUCE_STANDALONE_APPLICATION=JucePlugin_Build_Standalone
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_VST3_CAN_REPLACE_VST2=0
    JucePlugin_AAXCategory=2048
    JucePlugin_AAXDisableBypass=0
    JucePlugin_AAXDisableMultiMono=0
    JucePlugin_Build_AAX=0
    JucePlugin_Build_AUv3=0
    JucePlugin_Build_RTAS=0
    JucePlugin_Build_Standalone=0
    JucePlugin_Build_Unity=0
    JucePlugin_Build_VST=0
    JucePlugin_Build_VST3=1
    JucePlugin_EditorRequiresKeyboardFocus=0
    JucePlugin_Enable_IAA=0
    JucePlugin_IAAType=0x666c7569
    JucePlugin_IsMidiEffect=0
    JucePlugin_IsSynth=1
    JucePlugin_ProducesMidiOutput=0
    JucePlugin_RTASCategory=2048
    JucePlugin_RTASDisableBypass=0
    JucePlugin_RTASDisableMultiMono=0
    JucePlugin_VSTCategory=kPlugCategSynth
    JucePlugin_Vst3Category=§Instrument/Synth§
    JucePlugin_VSTNumMidiInputs=16
    JucePlugin_VSTNumMidiOutputs=16
    JucePlugin_WantsMidiInput=1
    PRIMITIVE_TYPES_ARE_INLINED
    UNICODE)

# --- add specific settings per platform ---
IF(WINDOWS)
    SET(cppDefineClauseList
        ${cppDefineClauseList}
        _CRT_SECURE_NO_WARNINGS
        JucePlugin_Build_AU=0
        NOMINMAX
        _WINDOWS
        _WINDLL
        WIN32)
ENDIF(WINDOWS)

IF(MACOSX)
    SET(cppDefineClauseList
        ${cppDefineClauseList}
        JucePlugin_Build_AU=1
        APPLE)

    # add 'aumu' as AU plugin type
    SET(manufacturerAndSuiteDefineClauseList
        ${manufacturerAndSuiteDefineClauseList}
        JucePlugin_AUMainType=0x61756c75
        JucePlugin_AUManufacturerCode=JucePlugin_ManufacturerCode)

    ENABLE_LANGUAGE(OBJC)
    SET(CMAKE_CXX_STANDARD_REQUIRED False)
ENDIF(MACOSX)

IF(LINUX)
    SET(cppDefineClauseList
        ${cppDefineClauseList}
        JucePlugin_Build_AU=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_USE_XCURSOR=0
        JUCE_USE_XINERAMA=0
        JUCE_USE_XRANDR=0
        JUCE_USE_XRENDER=0
        LINUX=1
        UNIX)
ENDIF(LINUX)

# --- combine defines into single list ---
SET(cppDefineClauseList
    ${cppDefineClauseList}
    ${manufacturerAndSuiteDefineClauseList})
  
# --- define flags per compiler ---
IF(MSVC)
    # --- list of warning number to be ignored
    SET(warningNumberList
        4100 4180 4244 4505 4723 5105 6011 6255 6297
        26439 26451 26495 26498 26812 26819 28182)
  
    # select static MSVC library instead of dynamic library
    # SET(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
  
    STRING(JOIN " " cppFlagsCommon
           /arch:AVX            # enable AVX vectorization instructions
           /bigobj              # increase number of addressable sections
           /diagnostics:column  # format of diagnostics message
           /EHsc                # exception handling: stack unwinding
           /Gd                  # cdecl calling convention
           /GS                  # buffers security check
           /MP                  # multi processor compilation
           /nologo              # suppress display of banner
           /permissive-         # set strict standard conformance
           /W4                  # warning level 4
           /Zc:forScope         # standard conformance for scoping
           /Zc:inline           # remove unreferenced functions
           /Zc:preprocessor     # conforming preprocessor
           /Zc:wchar_t          # wchar is native
    )

    # --- disable all warnings in warningNumberList ---
    FOREACH(warningNumber ${warningNumberList})
        STRING(APPEND cppFlagsCommon " /wd${warningNumber}")
    ENDFOREACH()         

    # ---  add all clauses in cppDefineClauseList ---
    FOREACH(defineClause ${cppDefineClauseList})
        STRING(REPLACE "§" "\\\"" cppDefinitionFlag " /D" ${defineClause})
        STRING(APPEND cppFlagsCommon ${cppDefinitionFlag})
    ENDFOREACH()         
  
    STRING(JOIN " " cppFlagsRelease
           /DNDEBUG      # no debugging
           /O2           # generate fast code
           /fp:fast      # fast floating point calculation
           /Gw           # global program optimization
    )

    STRING(JOIN " " cppFlagsReleaseWithDebugInfo
           /DDEBUG       # debugging
           /Od           # no optimization
           /Zi           # debug information in database
           /fp:fast      # fast floating point calculation
    )

    STRING(JOIN " " cppFlagsDebug
           /DDEBUG       # debugging on
           /Od           # no optimization
           /Zi           # debug information in database
           /fp:fast      # fast floating point calculation
    )
ELSE()
    STRING(JOIN " " cppFlagsCommon
           -ffast-math                   # fast floating point calculation
           -mavx                         # enable AVX vectorization
                                         # instructions
           -O0                           # no optimization
           -Ofast                        # favors fast code
           -pedantic                     # set strict standard conformance
           -Wall                         # warning level: all
           -Wno-delete-incomplete        # remove warning for void deletion
           -Wno-ignored-qualifiers       # remove warning for const qualifier
                                         # on functions
           -Wno-unused-function          # remove warning for unused function
           -Wno-unused-variable          # remove warning for unused variable
           -Wno-unused-but-set-variable  # remove warning for unused variable
           )

    # warn about undefined symbols when linking
    IF(MACOSX)
        STRING(JOIN " " cppLinkerFlagsCommon
               ${cppLinkerFlagsCommon}
               -Wl,-undefined,error)
    ELSE()
        STRING(JOIN " " cppLinkerFlagsCommon
               ${cppLinkerFlagsCommon}
               -Wl,-no-undefined)
    ENDIF(MACOSX)           

    # ---  add all clauses in cppDefineClauseList ---
    FOREACH(defineClause ${cppDefineClauseList})
        STRING(REPLACE "§" "\\\"" cppDefinitionFlag " -D" ${defineClause})
        STRING(APPEND cppFlagsCommon ${cppDefinitionFlag})
    ENDFOREACH()         
  
    STRING(JOIN " " cppFlagsRelease
           -DNDEBUG         # no debugging
           -O3              # extreme optimization
    )

    STRING(JOIN " " cppFlagsReleaseWithDebugInfo
           -DNDEBUG         # no debugging
           -O3              # extreme optimization
    )

    STRING(JOIN " " cppFlagsDebug
           -DDEBUG              # debugging on
           -g                   # debug information in object files
    )
ENDIF()

SET(CMAKE_CXX_FLAGS ${cppFlagsCommon} CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS_RELEASE ${cppFlagsRelease} CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${cppFlagsReleaseWithDebugInfo}
    CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG ${cppFlagsDebug} CACHE STRING "" FORCE)

SET(CMAKE_SHARED_LINKER_FLAGS ${cppLinkerFlagsCommon} CACHE STRING "" FORCE)
