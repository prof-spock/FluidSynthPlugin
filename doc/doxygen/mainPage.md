\mainpage FluidSynth Plugin for DAWs and Pedantic FluidSynth Command Line Processor
<DIV>
    <B>author:</B> Dr. Thomas Tensi<BR>
    <B>date:</B> November 2022
</DIV>

<H1>Overview</H1>

<U><A HREF="https://www.fluidsynth.org">FluidSynth</A></U> is one of
the most prominent open source MIDI players.  It is reasonably
flexible, delivers a good audio quality and is available for the
typical platforms.  A common scenario is to use it for either
rendering live MIDI data on some audio device or converting MIDI files
into audio files by command-line batch processing.

Basis of <TT>FluidSynth</TT> are the so-called *soundfonts*.
Soundfonts contain sampled instruments together with envelope and
modulation definitions and other descriptive settings.  It is easy to
find really usable ones in the internet and also several of those
cover all general MIDI instruments (for example, the
<TT>FluidR3_GM.sf2</TT>).

This software package provides two components:
<UL>
    <LI>The first component is a DAW plugin called
        <B><TT>FluidSynthPlugin</TT></B>.  It has a simplistic
        interface where you specify a soundfont, several fluidsynth
        settings and possibly a MIDI program to be selected by putting
        text data in a text field.  Then you are able to convert an
        incoming MIDI stream in a DAW to audio using the FluidSynth
        library.</LI>

    <LI>The second component is a simplistic but pedantic command-line
        converter called <B><TT>FluidSynthConverter</TT></B>.  It
        circumvents the rasterization by the player of the original
        command-line program <TT>FluidSynth</TT>.  It converts a MIDI
        file into a WAV file, is also based on the fluidsynth library
        and does the same sample-exact event feeding into that library
        as the plugin above.</LI>
</UL>

When using both components (command-line and DAW) on the same MIDI
data they typically produce audio output with a difference of less
than -200dBFS in a spectrum analysis.

This doxygen documentation gives a detailed description of the C++
sources for the FluidSynthPlugin and the FluidSynthConverter.  Note
that the implementation is completely free, open-source,
platform-neutral and based on the <U><A HREF="https://juce.com">JUCE
audio framework</A></U>.  Those components are currently available
only for Windows as VST3, but the MacOS and Linux versions are in
preparation.  Porting to other targets should be straightforward,
since building is supported by a platform-neutral CMAKE build file.

<H1>Effects Provided</H1>

The following effects are available in this package:
<UL>

  <LI><A HREF="namespace_main_1_1_fluid_synth_plugin.html">
      <B>FluidSynthPlugin</B></A>: the DAW plugin for MIDI to audio
      conversion using the FluidSynth library.</LI>

  <LI><A HREF="namespace_main_1_1_fluid_synth_file_converter.html">
      <B>FluidSynthFileConverter</B></A>: the command line converter
      for MIDI to audio conversion using the FluidSynth library and
      reimplementing the FluidSynth command-line processor with a
      stricter MIDI event placement.</LI>

</UL>