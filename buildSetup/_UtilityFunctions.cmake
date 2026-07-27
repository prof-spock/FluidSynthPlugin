# -*- coding: utf-8 -*-
#
# utitilty functions in CMAKE for booleans, lists etc.

#################
### FUNCTIONS ###
#################

#--------------------
# Booleans
#--------------------

MACRO(UTIL_Bool_setToInverse
      resultVariableName value)
    # sets <resultVariableName> to boolean inverse of <value>

    IF(${value})
        SET(${resultVariableName} FALSE)
    ELSE()
        SET(${resultVariableName} TRUE)
    ENDIF()
ENDMACRO(UTIL_Bool_setToInverse)

#--------------------

MACRO(UTIL_Bool_setToZeroOrOne
      resultVariableName value)
    # sets variable named <resultVariableName> to 1 if <value> is true
    # else to 0

    IF(${value})
        SET(${resultVariableName} 1)
    ELSE()
        SET(${resultVariableName} 0)
    ENDIF()
ENDMACRO(UTIL_Bool_setToZeroOrOne)

#--------------------
# Debugging Support
#--------------------

MACRO(UTIL_Debug_appendRelevantVariableNames )
    # appends all variable names given as ARGV to
    # <relevantVariableNameList>

    LIST(APPEND relevantVariableNameList ${ARGV})
ENDMACRO(UTIL_Debug_appendRelevantVariableNames)

#--------------------
# Lists
#--------------------

MACRO(UTIL_List_appendConditionally
      resultVariableName conditionVariableName st)
    # appends string <st> to variable named <resultVariableName> when
    # variable named <conditionVariableName> is set

    IF(DEFINED ${conditionVariableName})
        IF(${${conditionVariableName}})
            LIST(APPEND ${resultVariableName} ${st})
        ENDIF()
    ENDIF()
ENDMACRO(UTIL_List_appendConditionally)

#--------------------

MACRO(UTIL_List_appendOtherTransformed
      resultVariableName listVariable prefix suffix)
    # sets variable named <resultVariableName> to list constructed by
    # iterating over <listVariable> adding <prefix> and <suffix> to
    # each entry

    FOREACH(element ${${listVariable}})
        LIST(APPEND ${resultVariableName} ${prefix}${element}${suffix})
    ENDFOREACH()         
ENDMACRO(UTIL_List_appendOtherTransformed)

#--------------------

MACRO(UTIL_List_constructFromOther
      resultVariableName listVariable prefix suffix)
    # sets variable named <resultVariableName> to list constructed by
    # iterating over <listVariable> adding <prefix> and <suffix> to
    # each entry

    SET(${resultVariableName})
    UTIL_List_appendOtherTransformed(${resultVariableName}
                                     ${listVariable}
                                     "${prefix}" "${suffix}")
ENDMACRO(UTIL_List_constructFromOther)

#--------------------

FUNCTION(UTIL_List_showContents
         variableNameList)
    # shows contents of variables with names in <variableNameList>

    SET(CMAKE_MESSAGE_INDENT "[variables] ")

    FOREACH(variableName ${variableNameList})
        IF(NOT DEFINED ${variableName})
            SET(value "---")
        ELSE()
            SET(value ${${variableName}})
        ENDIF()

        MESSAGE(STATUS "${variableName}: ${value}")
    ENDFOREACH()
ENDFUNCTION(UTIL_List_showContents)

#--------------------
# Targets
#--------------------

MACRO(UTIL_Target_setFolder
      targetName folderName)
    # sets folder of <targetName> to <folderName>

    SET_TARGET_PROPERTIES(${targetName} PROPERTIES FOLDER "${folderName}")
ENDMACRO(UTIL_Target_setFolder)
