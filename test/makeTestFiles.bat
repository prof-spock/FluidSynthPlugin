@ECHO OFF
REM makeTestFiles - renders several MIDI files with standard FluidSynth
REM                 and pedantic FluidSynth File Converter

SET scriptDirectory=%~dp0.

SET configuration=Release
IF NOT "%1"=="" SET configuration=%1

SET platform=Windows-AMD64\%configuration%
SET sampleRateList=44100 48000

REM === path of standard FluidSynth ===
SET fsProgram=CALL fluidsynth

REM === path of (pedantic) fluidsynth file converter ===
SET fsfcProgram=%scriptDirectory%\..\targetPlatforms\%platform%
SET fsfcProgram=%fsfcProgram%\FluidSynthFileConverter
SET fsfcProgram=%fsfcProgram%\FluidSynthFileConverter

SET soundFont=SimpleTestSoundfont.SF2
SET prefixList=test_bass test_drums test_keyboard

PUSHD %scriptDirectory%
FOR %%i IN (%prefixList%) DO CALL :renderMidiFile %%i
POPD

GOTO :EOF

REM ============================================================

:renderMidiFile
    SET prefix=%1

    REM iterate over all sample rates
    FOR %%s IN (%sampleRateList%) DO CALL :renderMidiFileWithSampleRate %prefix% %%s
GOTO :EOF

REM --------------------

:renderMidiFileWithSampleRate
    SET prefix=%1
    SET sampleRate=%2

    SET renderOptions=-R 0 -C 0 -g 1.0 -O float -r %sampleRate%

    SET midiFile=%prefix%.mid
    SET inputFileList=%midiFile% %soundFont%

    SET waveFile=FSFC-%prefix%-%sampleRate%.wav
    ECHO == render %waveFile% with pedantic converter
    %fsfcProgram% %renderOptions% -F %waveFile% %inputFileList%

    SET waveFile=FS-%prefix%-%sampleRate%.wav
    ECHO == render %waveFile% with standard fluidsynth
    %fsProgram%  -q -n -i %renderOptions% -F %waveFile% %inputFileList%
GOTO :EOF
