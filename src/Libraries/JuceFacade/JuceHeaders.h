#pragma once
#define DONT_SET_USING_JUCE_NAMESPACE 1

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#if ! JUCE_DONT_DECLARE_PROJECTINFO
namespace ProjectInfo
{
    const char* const  projectName    = "FluidSynthPlugin";
    const char* const  companyName    = JucePlugin_Manufacturer;
    const char* const  versionString  = JucePlugin_VersionString;
    const int          versionNumber  = JucePlugin_VersionCode;
}
#endif

/*--------------------*/
/* helper definitions */
/*--------------------*/

/** combines <C>className</C> and <C>prefix</C> into a JUCE colour id
  * name */
#define colourId2(className, prefix) \
    juce::className::ColourIds::prefix##ColourId

/** combines <C>className</C>, <C>prefix</C> and <C>partText</C> into
  * a JUCE colour id name */
#define colourId3(className, prefix, partText) \
    juce::className::ColourIds::prefix##Colour##partText##Id

/*----------------------*/
/* adaptation functions */
/*----------------------*/
/* those help to encapsulate irrelevant changes in the JUCE API between
   major versions */

#if JUCE_MAJOR_VERSION < 8
    /** makes font from <C>size</C> and <C>style</C>*/
    #define JuceFont_make(size, style) \
        juce::Font{(float) (size), style}

    /** returns width of string <C>st</C> in <C>font</C> in pixels */
    #define JuceFont_getStringWidth(font, st) \
        font.getStringWidth(st)
#else
    /** makes font from <C>size</C> and <C>style</C>*/
    #define JuceFont_make(size, style) \
        juce::Font{juce::FontOptions{(float) (size), style}}

    /** returns width of string <C>st</C> in <C>font</C> in pixels */
    #define JuceFont_getStringWidth(font, st) \
        juce::GlyphArrangement::getStringWidth((font), (st))
#endif
