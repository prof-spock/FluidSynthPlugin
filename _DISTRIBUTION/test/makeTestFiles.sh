#!/bin/bash
# makeTestFiles - renders several MIDI files with standard FluidSynth
#                 and pedantic FluidSynth File Converter

# ============================================================

function renderMidiFile(prefix, \
                        inputFileList, midiFile, renderOptions, waveFile) {
    midiFile=${prefix}.mid
    renderOptions=-R 0 -C 0 -g 1.0 -O float
    inputFileList=${midiFile} ${soundFont}

    waveFile=FSFC-${prefix}.wav
    echo "== render ${waveFile}"
    ${fsfcProgram} ${renderOptions} -F ${waveFile} ${inputFileList}

    waveFile=FS-${prefix}.wav
    echo "== render ${waveFile}"
    ${fsProgram}  -q -n -i ${renderOptions} -F ${waveFile} ${inputFileList}
}

# ============================================================

# selected platform "Windows-AMD64", "Darwin-x86_64" or "Linux-x86_64"
platform=Windows-AMD64

# === standard FluidSynth ===
fsProgram=fluidsynth

# === (Pedantic) FluidSynth File Converter ===
fsfcProgram=../_DISTRIBUTION/targetPlatforms/${platform}
fsfcProgram=${fsfcProgram}/FluidSynthFileConverter
fsfcProgram=${fsfcProgram}/FluidSynthFileConverter

soundFont=SimpleTestSoundfont.SF2
prefixList=test_bass test_drums test_keyboard

for prefix in ${prefixList} do
    renderMidiFile(prefix)
done
