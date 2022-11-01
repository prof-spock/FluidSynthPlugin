/**
 * @file
 * The <C>JucePluginDefines</C> sets up effect parameters for the JUCE
 * framework.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

#pragma once

/* Plugin Settings */

/** the fully qualified name of the plugin */
#define _PluginPathName                  eu.tensi.FluidSynthPlugin

/**  the simple name of the plugin */
#define JucePlugin_Name                  "FluidSynthPlugin"

/** the description of the plugin */
#define JucePlugin_Desc                  "Fluidsynth for a DAW"

/** the four byte code for the plugin 'Flui' */
#define JucePlugin_PluginCode            0x466c7569

/* settings for the different plugin kinds */

/** the name of the plugin for AAX */
#define JucePlugin_AAXIdentifier         _PluginPathName

/** the four byte code for the plugin for AAX */
#define JucePlugin_AAXProductId          JucePlugin_PluginCode

/** the prefix for the plugin in the description file */
#define JucePlugin_AUExportPrefix        FluidSynthPluginAU

/** the prefix for AU plugins */
#define JucePlugin_AUExportPrefixQuoted  "FluidSynthPluginAU"

/** the four byte code for the plugin for AU */
#define JucePlugin_AUSubType             JucePlugin_PluginCode

/** the bundle path for MacOSX */
#define JucePlugin_CFBundleIdentifier    _PluginPathName

/** the name for IAA plugins */
#define JucePlugin_IAAName               "DrTT: FluidSynthPlugin"

/** the subtype for IAA plugins (four byte code) */
#define JucePlugin_IAASubType            JucePlugin_PluginCode

/** the product ID for RTAS plugins (four byte code) */
#define JucePlugin_RTASProductId         JucePlugin_PluginCode

/** the four byte unique ID for VST plugins */
#define JucePlugin_VSTUniqueID           JucePlugin_PluginCode
