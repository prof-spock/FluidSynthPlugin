# -*- coding: utf-8 -*-
#
# Local Settings for the C++-Compiler in CMAKE for DrTT JUCE plugins
#

# #################
# ### FUNCTIONS ###
# #################

FUNCTION(CMPCONFP_appendToCommonDefineClauses)
    # appends JUCE settings to all define clauses common to all build
    # configurations

    LIST(APPEND CMPCONF_cppDefinitions_common
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
         JUCE_USE_XINPUT=0
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
         JUCE_WEB_BROWSER=0
    )
    
    # --- add specific settings per platform ---
    IF(WINDOWS)
        LIST(APPEND CMPCONF_cppDefinitions_common
             JucePlugin_Build_AU=0
        )
    ENDIF(WINDOWS)

    IF(MACOS)
        LIST(APPEND CMPCONF_cppDefinitions_common
             JucePlugin_Build_AU=1
             JUCE_MAC=1
        )
    ENDIF(MACOS)

    IF(LINUX)
        LIST(APPEND CMPCONF_cppDefinitions_common
             JucePlugin_Build_AU=0
             JUCE_WEB_BROWSER=0
             JUCE_USE_CURL=0
             JUCE_USE_XCURSOR=0
             JUCE_USE_XINERAMA=0
             JUCE_USE_XRANDR=0
             JUCE_USE_XRENDER=0
        )
    ENDIF(LINUX)

    CMPCONFP_appendToManufacturerAndSuiteDefineClauses()

    # --- combine defines into single list ---
    LIST(APPEND CMPCONF_cppDefinitions_common
         ${CMPCONF_manufacturerAndSuiteDefineClauseList})

    SET(CMPCONF_cppDefinitions_common
        ${CMPCONF_cppDefinitions_common} PARENT_SCOPE)
    SET(CMPCONF_manufacturerAndSuiteDefineClauseList
        ${CMPCONF_manufacturerAndSuiteDefineClauseList} PARENT_SCOPE)
ENDFUNCTION(CMPCONFP_appendToCommonDefineClauses)

#--------------------

FUNCTION(CMPCONFP_appendToManufacturerAndSuiteDefineClauses)
    # appends define clauses for manufacturer and effects suite

    # add global manufacturer settings
    LIST(APPEND CMPCONF_manufacturerAndSuiteDefineClauseList
         JucePlugin_Manufacturer="DrTT"
         JucePlugin_ManufacturerEmail=""
         JucePlugin_ManufacturerCode=0x44725454
         JucePlugin_AAXManufacturerCode=JucePlugin_ManufacturerCode
    )

    IF(MACOS)
        LIST(APPEND CMPCONF_manufacturerAndSuiteDefineClauseList
             JucePlugin_AUManufacturerCode=JucePlugin_ManufacturerCode
        )
    ENDIF(MACOS)

    SET(CMPCONF_manufacturerAndSuiteDefineClauseList
        ${CMPCONF_manufacturerAndSuiteDefineClauseList} PARENT_SCOPE)
ENDFUNCTION(CMPCONFP_appendToManufacturerAndSuiteDefineClauses)

# ########################################

# --- append to settings from specific effect suite
CMPCONFP_appendToCommonDefineClauses()
