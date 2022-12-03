#!/bin/bash
# makeTestFiles - renders several MIDI files with standard FluidSynth
#                 and pedantic FluidSynth File Converter

# selected platform "Windows-AMD64", "Darwin-x86_64" or "Linux-x86_64"
# platform=Windows-AMD64
platform=Linux-x86_64

renderOptions="-R 0 -C 0 -g 1.0 -O float"
soundFont="SimpleTestSoundfont.SF2"

# === standard FluidSynth ===
fsProgram=fluidsynth

# === (Pedantic) FluidSynth File Converter ===
fsfcProgram=../_DISTRIBUTION/targetPlatforms/${platform}
fsfcProgram=${fsfcProgram}/FluidSynthFileConverter
fsfcProgram=${fsfcProgram}/FluidSynthFileConverter

# ============================================================

function renderMidiFile () {
    prefix=$1

    midiFile="${prefix}.mid"
    inputFileList="${midiFile} ${soundFont}"

    waveFile=FSFC-${prefix}.wav
    echo "== render ${waveFile}"
    ${fsfcProgram} ${renderOptions} -F ${waveFile} ${inputFileList}

    waveFile=FS-${prefix}.wav
    echo "== render ${waveFile}"
    ${fsProgram}  -q -n -i ${renderOptions} -F ${waveFile} ${inputFileList}
}

# ============================================================

prefixList="test_bass test_drums test_keyboard"

for prefix in ${prefixList}; do
    renderMidiFile ${prefix}
done
