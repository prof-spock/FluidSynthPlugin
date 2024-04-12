# -*- coding: utf-8 -*-
#
# Local Settings for the C++-Compiler in CMAKE for DrTT JUCE plugins
#

# #################
# ### FUNCTIONS ###
# #################

IF(MSVC)
    SET(Warning_disablingPrefix /wd)
    SET(Warning_disableAll /W0)
    SET(Warning_maximumLevel /W4)
ELSE()
    SET(Warning_disablingPrefix -Wno-)
    SET(Warning_disableAll -w)
    SET(Warning_maximumLevel -Wall)
ENDIF()


FUNCTION(appendToCommonDefineClauses)
    # appends all define clauses common to all build configurations

    LIST(APPEND cppDefinitions_common
        _LIB
        _UNICODE
        JUCE_DISPLAY_SPLASH_SCREEN=1
        JUCE_DONT_DECLARE_PROJECTINFO=1
        JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
        JUCE_MODAL_LOOPS_PERMITTED=1
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
        JUCE_REPORT_APP_USAGE=0
        JUCE_SHARED_CODE=1
        JUCE_STANDALONE_APPLICATION=JucePlugin_Build_Standalone
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_USE_DARK_SPLASH_SCREEN=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JucePlugin_AAXDisableBypass=0
        JucePlugin_AAXDisableMultiMono=0
        JucePlugin_Build_AAX=0
        JucePlugin_Build_AUv3=0
        JucePlugin_Build_RTAS=0
        JucePlugin_Build_Standalone=0
        JucePlugin_Build_Unity=0
        JucePlugin_Build_VST3=1
        JucePlugin_Build_VST=0
        JucePlugin_EditorRequiresKeyboardFocus=0
        JucePlugin_Enable_IAA=0
        PRIMITIVE_TYPES_ARE_INLINED
        UNICODE
    )
    
    # --- add specific settings per platform ---
    IF(WINDOWS)
        LIST(APPEND cppDefinitions_common
             _CRT_SECURE_NO_WARNINGS
             JucePlugin_Build_AU=0
             NOMINMAX
             _WINDOWS
             _WINDLL
             WIN32
        )
    ENDIF(WINDOWS)

    IF(MACOSX)
        LIST(APPEND cppDefinitions_common
             JucePlugin_Build_AU=1
             JUCE_MAC=1
             APPLE
        )
    ENDIF(MACOSX)

    IF(LINUX)
        LIST(APPEND cppDefinitions_common
             JucePlugin_Build_AU=0
             JUCE_WEB_BROWSER=0
             JUCE_USE_CURL=0
             JUCE_USE_XCURSOR=0
             JUCE_USE_XINERAMA=0
             JUCE_USE_XRANDR=0
             JUCE_USE_XRENDER=0
             LINUX=1
             UNIX
        )
    ENDIF(LINUX)

    appendToManufacturerAndSuiteDefineClauses()

    # --- combine defines into single list ---
    LIST(APPEND cppDefinitions_common
         ${manufacturerAndSuiteDefineClauseList})

    SET(cppDefinitions_common ${cppDefinitions_common} PARENT_SCOPE)
ENDFUNCTION(appendToCommonDefineClauses)

#--------------------

FUNCTION(appendToManufacturerAndSuiteDefineClauses)
    # appends define clauses for manufacturer and effects suite

    # add global manufacturer settings
    LIST(APPEND manufacturerAndSuiteDefineClauseList
         JucePlugin_Manufacturer="DrTT"
         JucePlugin_ManufacturerEmail=""
         JucePlugin_ManufacturerCode=0x44725454
         JucePlugin_AAXManufacturerCode=JucePlugin_ManufacturerCode
    )

    IF(MACOSX)
        LIST(APPEND manufacturerAndSuiteDefineClauseList
             JucePlugin_AUManufacturerCode=JucePlugin_ManufacturerCode
        )
    ENDIF(MACOSX)

    SET(manufacturerAndSuiteDefineClauseList
        ${manufacturerAndSuiteDefineClauseList} PARENT_SCOPE)
ENDFUNCTION(appendToManufacturerAndSuiteDefineClauses)

#--------------------

FUNCTION(setSupportedProgrammingLanguages)
    # defines programming languages supported for this build

    SET(CMAKE_CXX_STANDARD 20 PARENT_SCOPE)
    SET(CMAKE_CXX_STANDARD_REQUIRED True PARENT_SCOPE)

    IF(MACOSX)
        ENABLE_LANGUAGE(OBJC)
    ENDIF(MACOSX)
ENDFUNCTION(setSupportedProgrammingLanguages)

#--------------------

FUNCTION(setCommonAndReleaseWarnings)
    # calculates warnings for common and release builds and returns
    # them as <cppWarningOptions_common> and <cppWarningOptions_release>

    IF(MSVC)
        # --- list of warning numbers to be ignored
        LIST(APPEND ignoredWarningList_common
              4100 # unreferenced formal parameter
              4244 # possible loss of data by onversion 
              4505 # unreferenced local function
              5105 # macro expansion producing 'defined' has undefined
                   # behavior
              6255 # _alloca indicates failure
              6297 # 32-bit value shifted to 64 bit
             26495 # uninitialized member variable
             26812 # enum type unscoped
        )

        LIST(APPEND ignoredWarningList_release
              4101 # unreferenced local variable
              4189 # variable declared and initialized but not used
        )
    ELSE()
        # --- list of warnings to be ignored
        LIST(APPEND ignoredWarningList_common
             address             # remove warning for impossible null
                                 # pointer
             delete-incomplete   # remove warning for void deletion
             ignored-qualifiers  # remove warning for const qualifier
                                 # on functions
             unused-function     # remove warning for unused function
        )

        LIST(APPEND ignoredWarningList_release
             unused-variable     # remove warning for unused variable
        )

        IF(CLANG)
            LIST(APPEND ignoredWarningList_common
                 ambiguous-reversed-operator # remove C++20 warning on
                                             # reversed equal operator
            )
        ELSE()
            LIST(APPEND ignoredWarningList_common
                 unused-but-set-variable  # remove warning for unused
                                          # variable
        )

        ENDIF(CLANG)
    ENDIF(MSVC)

    LIST(APPEND cppWarningOptions_common ${Warning_maximumLevel})

    FOREACH(warning ${ignoredWarningList_common})
        LIST(APPEND cppWarningOptions_common
             ${Warning_disablingPrefix}${warning})
    ENDFOREACH()         

    SET(cppWarningOptions_release)

    FOREACH(warning ${ignoredWarningList_release})
        LIST(APPEND cppWarningOptions_release
            ${Warning_disablingPrefix}${warning})
    ENDFOREACH()

    SET(cppWarningOptions_common ${cppWarningOptions_common} PARENT_SCOPE)
    SET(cppWarningOptions_release ${cppWarningOptions_release} PARENT_SCOPE)
ENDFUNCTION(setCommonAndReleaseWarnings)

# ########################################

IF (${CMAKE_CXX_COMPILER_ID} MATCHES ".*Clang")
    SET(CLANG 1)
ELSE()
    SET(CLANG 0)
ENDIF()

# --- set languages to C++ and Objective-C (for MacOSX)
setSupportedProgrammingLanguages()

# --- append to settings from specific effect suite
appendToCommonDefineClauses()

# --- collect warnings
setCommonAndReleaseWarnings()

# --- define flags per compiler ---
IF(MSVC)
    LIST(APPEND cppOptions_common
         /bigobj              # increase number of addressable sections
         /diagnostics:column  # format of diagnostics message
         /EHsc                # exception handling: stack unwinding
         /fp:fast             # fast floating point calculation
         /Gd                  # cdecl calling convention
         /GS                  # buffers security check
         /MP                  # multi processor compilation
         /nologo              # suppress display of banner
         /permissive-         # set strict standard conformance
         /Zc:forScope         # standard conformance for scoping
         /Zc:inline           # remove unreferenced functions
         /Zc:preprocessor     # conforming preprocessor
         /Zc:wchar_t          # wchar is native
    )

    LIST(APPEND cppOptions_release
         /Gw                  # global program optimization
         /O2                  # generate fast code
         /Qpar                # enables loop parallelization
    )

    LIST(APPEND cppOptions_relWithDebInfo
         /Z7                  # debug information in file
         /Od                  # no optimization
    )

    LIST(APPEND cppOptions_debug
         /Od                  # no optimization
         /Zi                  # debug information in database
    )
ELSE()
    LIST(APPEND cppOptions_common
         -ffast-math          # fast floating point calculation
         -O0                  # no optimization
         -Ofast               # favors fast code
         -pedantic            # set strict standard conformance
    )

    IF(MACOSX)
        LIST(APPEND cppOptions_common
             -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk)
    ENDIF(MACOSX)

    # warn about undefined symbols when linking
    IF(MACOSX)
        LIST(APPEND cppLinkerOptions_common
             -Wl,-undefined,error)
    ELSE()
        LIST(APPEND cppLinkerOptions_common
             -Wl,-no-undefined)
    ENDIF(MACOSX)           

    LIST(APPEND cppOptions_release
         -O3                  # extreme optimization
    )

    LIST(APPEND cppOptions_relWithDebInfo
         -Og                  # debugging compatible optimization
         -g                   # debug information in object files
    )

    LIST(APPEND cppOptions_debug
         -Og                  # debugging compatible optimization
         -g                   # debug information in object files
    )
ENDIF()

LIST(APPEND cppWarningOptions
     ${cppWarningOptions_common}
     $<$<CONFIG:Release>:${cppWarningOptions_release}>)

SET(CMAKE_SHARED_LINKER_FLAGS ${cppLinkerOptions_common} CACHE STRING "" FORCE)

SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO CACHE STRING "" FORCE)

#====================

FUNCTION(addCompilerFlags targetName warningsAreEnabled)
    # adds compiler flags to target <targetName> taking into account
    # whether warnings are enabled via <warningsAreEnabled>
    MESSAGE(STATUS
            "target " ${targetName}
            ": compiler warnings = " ${warningsAreEnabled})

    # definitions
    TARGET_COMPILE_DEFINITIONS(${targetName} PRIVATE
                               ${cppDefinitions_common})

    TARGET_COMPILE_DEFINITIONS(${targetName} PRIVATE
                               $<IF:$<CONFIG:Release>,NDEBUG,DEBUG>)

    # compile with logging enabled globally for a debug configuration
    # TARGET_COMPILE_DEFINITIONS(${targetName} PRIVATE
    #                            $<$<CONFIG:Debug>:LOGGING_IS_ACTIVE>)

    # options
    TARGET_COMPILE_OPTIONS(${targetName} PRIVATE ${cppOptions_common})

    IF(warningsAreEnabled)
        TARGET_COMPILE_OPTIONS(${targetName} PRIVATE ${cppWarningOptions})
    ELSE()
        TARGET_COMPILE_OPTIONS(${targetName} PRIVATE ${Warning_disableAll})
    ENDIF()
ENDFUNCTION(addCompilerFlags)
