#
# Local Settings for CMAKE for the FluidSynthPlugin
#

SET(programDirectory "/Program Files")

# the directory where pdflatex is located
IF(NOT LATEX_PDFLATEX_FOUND)
    SET(PDF_LATEX_COMPILER
        ${programDirectory}/latex/bin/pdflatex)
    SET(LATEX_PDFLATEX_FOUND 1)
ENDIF()

# the directory where doxygen is located
IF(NOT DOXYGEN_FOUND)
    SET(DOXYGEN_EXECUTABLE
        ${programDirectory}/DoxyGen/doxygen)
    SET(DOXYGEN_FOUND 1)

    # graphviz support
    SET(DOXYGEN_DOT_PATH ${programDirectory}/Graphviz/bin)
    SET(DOXYGEN_HAVE_DOT YES)
ENDIF()

# the directory where the JUCE modules are located
IF(NOT JUCE_FOUND)
    SET(JUCE_DIR ${programDirectory}/JUCE)
ENDIF()
