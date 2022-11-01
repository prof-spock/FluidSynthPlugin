@ECHO OFF
REM makeTestFiles - renders several MIDI files with standard FluidSynth
REM                 and pedantic FluidSynth File Converter

SET platform=Windows-AMD64\Release
SET renderOptions=-R 0 -C 0 -g 1.0 -O float
REM SET renderOptions=-R 0 -C 0 -g 1.0

REM === standard FluidSynth ===
SET fsProgram=CALL fluidsynth

REM === (Pedantic) FluidSynth File Converter ===
SET fsfcProgram=..\_DISTRIBUTION\targetPlatforms\%platform%
SET fsfcProgram=%fsfcProgram%\FluidSynthFileConverter
SET fsfcProgram=%fsfcProgram%\FluidSynthFileConverter

SET soundFont=SimpleTestSoundfont.SF2
SET prefixList=test_bass test_drums test_keyboard

FOR %%i IN (%prefixList%) DO CALL :renderMidiFile %%i
GOTO :EOF

REM ============================================================

:renderMidiFile
    SET prefix=%1
    SET midiFile=%prefix%.mid
    SET inputFileList=%midiFile% %soundFont%

    SET waveFile=FSFC-%prefix%.wav
    ECHO == render %waveFile%
    %fsfcProgram% %renderOptions% -F %waveFile% %inputFileList%

    SET waveFile=FS-%prefix%.wav
    ECHO == render %waveFile%
    %fsProgram%  -q -n -i %renderOptions% -F %waveFile% %inputFileList%
GOTO :EOF
