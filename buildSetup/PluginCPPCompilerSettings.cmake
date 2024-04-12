# -*- coding: utf-8 -*-
#
# Local Settings for the C++-Compiler in CMAKE for the
# FluidSynthPlugins

# plugin suite settings
LIST(APPEND manufacturerAndSuiteDefineClauseList
     JucePlugin_Version=1.0.0
     JucePlugin_VersionCode=0x10000
     JucePlugin_VersionString="1.0.0"
     JucePlugin_ManufacturerWebsite="https://github.com/prof-spock/FluidSynthPlugin"
)

IF(MACOSX)
    # add 'aumu' as AU plugin type
    LIST(APPEND manufacturerAndSuiteDefineClauseList
         JucePlugin_AUMainType=0x61756c75
    )
ENDIF(MACOSX)

LIST(APPEND cppDefinitions_common
     JUCE_APP_VERSION=1.0.0
     JUCE_APP_VERSION_HEX=0x10000
     JucePlugin_AAXCategory=2048
     JucePlugin_IAAType=0x666c7569
     JucePlugin_IsMidiEffect=0
     JucePlugin_IsSynth=1
     JucePlugin_ProducesMidiOutput=0
     JucePlugin_RTASCategory=2048
     JucePlugin_RTASDisableBypass=0
     JucePlugin_RTASDisableMultiMono=0
     JucePlugin_Vst3Category="Instrument/Synth"
     JucePlugin_VSTCategory=kPlugCategSynth
     JucePlugin_VSTNumMidiInputs=16
     JucePlugin_VSTNumMidiOutputs=16
     JucePlugin_WantsMidiInput=1
)
