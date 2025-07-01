# -*- coding: utf-8 -*-
#
# Local Settings for the C++-Compiler in CMAKE for DrTT JUCE programs
#

SET(CMPCONF_compilerIsCLANG 0)
SET(CMPCONF_compilerIsGCC   0)
SET(CMPCONF_compilerIsMSVC  0)

IF (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    SET(CMPCONF_compilerIsCLANG 1)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(CMPCONF_compilerIsGCC   1)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    SET(CMPCONF_compilerIsMSVC  1)
ENDIF()

IF(CMPCONF_compilerIsMSVC)
    SET(CMPCONF_Warning_disablingPrefix /wd)
    SET(CMPCONF_Warning_disableAll /W0)
    SET(CMPCONF_Warning_maximumLevel /W4)
ELSE()
    SET(CMPCONF_Warning_disablingPrefix -Wno-)
    SET(CMPCONF_Warning_disableAll -w)
    SET(CMPCONF_Warning_maximumLevel -Wall)
ENDIF()

# #################
# ### FUNCTIONS ###
# #################

FUNCTION(CMPCONF_addCompilerFlags targetName warningsAreEnabled)
    # adds compiler flags to target <targetName> taking into account
    # whether warnings are enabled via <warningsAreEnabled>

    MESSAGE(STATUS
            "target " ${targetName}
            ": compiler warnings = " ${warningsAreEnabled})

    # definitions
    TARGET_COMPILE_DEFINITIONS(${targetName} PRIVATE
                               ${CMPCONF_cppDefinitions_common})

    TARGET_COMPILE_DEFINITIONS(${targetName} PRIVATE
                               $<IF:$<CONFIG:Release>,NDEBUG,DEBUG>)

    IF(CMPCONF_debugVersionHasLogging STREQUAL "X")
        # compile with logging enabled globally for a debug
        # configuration
        TARGET_COMPILE_DEFINITIONS(${targetName} PRIVATE
                                   $<$<CONFIG:Debug>:LOGGING_IS_ACTIVE>)
    ENDIF()

    # options
    TARGET_COMPILE_OPTIONS(${targetName} PRIVATE
                           ${CMPCONF_cppOptions_common})

    IF(warningsAreEnabled)
        TARGET_COMPILE_OPTIONS(${targetName} PRIVATE
                               ${CMPCONF_cppWarningOptions})
    ELSE()
        TARGET_COMPILE_OPTIONS(${targetName} PRIVATE
                               ${CMPCONF_Warning_disableAll})
    ENDIF()

    TARGET_COMPILE_OPTIONS(${targetName} PRIVATE
                           $<$<CONFIG:Debug>:${CMPCONF_cppOptions_debug}>)
    TARGET_COMPILE_OPTIONS(${targetName} PRIVATE
                           $<$<CONFIG:Release>:${CMPCONF_cppOptions_release}>)
    TARGET_COMPILE_OPTIONS(${targetName} PRIVATE
                           $<$<CONFIG:RelWithDebInfo>:${CMPCONF_cppOptions_relWithDebInfo}>)
ENDFUNCTION(CMPCONF_addCompilerFlags)

#--------------------

FUNCTION(CMPCONF_appendToCommonDefineClauses)
    # appends all define clauses common to all build configurations

    LIST(APPEND CMPCONF_cppDefinitions_common
        _LIB
        _UNICODE
        PRIMITIVE_TYPES_ARE_INLINED
        UNICODE
    )
    
    # --- add specific settings per platform ---
    IF(WINDOWS)
        LIST(APPEND CMPCONF_cppDefinitions_common
             _CRT_SECURE_NO_WARNINGS
             NOMINMAX
             _WINDOWS
             _WINDLL
             WIN32
        )
    ENDIF(WINDOWS)

    IF(MACOS)
        LIST(APPEND CMPCONF_cppDefinitions_common
             APPLE
        )
    ENDIF(MACOS)

    IF(LINUX)
        LIST(APPEND CMPCONF_cppDefinitions_common
             LINUX=1
             UNIX
        )
    ENDIF(LINUX)

    # --- combine defines into single list ---
    LIST(APPEND CMPCONF_cppDefinitions_common
         ${manufacturerAndSuiteDefineClauseList})

    SET(CMPCONF_cppDefinitions_common
        ${CMPCONF_cppDefinitions_common} PARENT_SCOPE)
ENDFUNCTION(CMPCONF_appendToCommonDefineClauses)

#--------------------

FUNCTION(CMPCONF_setSupportedProgrammingLanguages)
    # defines programming languages supported for this build

    SET(CMAKE_CXX_STANDARD 20 PARENT_SCOPE)
    SET(CMAKE_CXX_STANDARD_REQUIRED True PARENT_SCOPE)

    IF(MACOS)
        ENABLE_LANGUAGE(OBJC)
    ENDIF(MACOS)
ENDFUNCTION(CMPCONF_setSupportedProgrammingLanguages)

#--------------------

FUNCTION(CMPCONF_setCommonAndReleaseWarnings)
    # calculates warnings for common and release builds and returns
    # them as <cppWarningOptions_common> and <cppWarningOptions_release>

    IF(CMPCONF_compilerIsMSVC)
        # --- list of warning numbers to be ignored
        SET(CMPCONF_ignoredWarningList_common
              4100 # unreferenced formal parameter
              4127 # constant boolean condition
              4146 # unary minus applied to unsigned type
              4244 # possible loss of data by onversion
              4458 # declaration hides class member
              4505 # unreferenced local function
              5105 # macro expansion producing 'defined' has undefined
                   # behavior
              6255 # _alloca indicates failure
              6297 # 32-bit value shifted to 64 bit
             26495 # uninitialized member variable
             26812 # enum type unscoped
        )

        SET(CMPCONF_ignoredWarningList_release
              4101 # unreferenced local variable
              4189 # variable declared and initialized but not used
        )
    ELSE()
        # --- list of warnings to be ignored
        SET(CMPCONF_ignoredWarningList_common
             address                 # remove warning for impossible null
                                     # pointer
             delete-incomplete       # remove warning for void deletion
             ignored-qualifiers      # remove warning for const qualifier
                                     # on functions
             unused-function         # remove warning for unused function
        )

        SET(CMPCONF_ignoredWarningList_release
             unused-variable         # remove warning for unused variable
        )

        IF(CMPCONF_compilerIsGCC)
            LIST(APPEND CMPCONF_ignoredWarningList_common
                 parentheses              # remove recommended parentheses
                 unused-but-set-variable  # remove warning for unused
                                          # variable
            )
        ENDIF()

        IF(CMPCONF_compilerIsCLANG)
            LIST(APPEND CMPCONF_ignoredWarningList_common
                 ambiguous-reversed-operator # remove C++20 warning on
                                             # reversed equal operator
                 logical-op-parentheses      # remove recommended parentheses
                                             # in logical expressions
            )
        ENDIF(CMPCONF_compilerIsCLANG)
    ENDIF(CMPCONF_compilerIsMSVC)

    LIST(APPEND CMPCONF_cppWarningOptions_common
         ${CMPCONF_Warning_maximumLevel})

    FOREACH(warning ${CMPCONF_ignoredWarningList_common})
        LIST(APPEND CMPCONF_cppWarningOptions_common
             ${CMPCONF_Warning_disablingPrefix}${warning})
    ENDFOREACH()         

    SET(CMPCONF_cppWarningOptions_release)

    FOREACH(warning ${CMPCONF_ignoredWarningList_release})
        LIST(APPEND CMPCONF_cppWarningOptions_release
            ${CMPCONF_Warning_disablingPrefix}${warning})
    ENDFOREACH()

    SET(CMPCONF_cppWarningOptions_common
        ${CMPCONF_cppWarningOptions_common} PARENT_SCOPE)
    SET(CMPCONF_cppWarningOptions_release
        ${CMPCONF_cppWarningOptions_release} PARENT_SCOPE)
ENDFUNCTION(CMPCONF_setCommonAndReleaseWarnings)

# ########################################

# --- set languages to C++ and Objective-C (for MacOS)
CMPCONF_setSupportedProgrammingLanguages()

# --- append to settings from specific effect suite
CMPCONF_appendToCommonDefineClauses()

# --- collect warnings
CMPCONF_setCommonAndReleaseWarnings()

# --- define flags per compiler ---
IF(CMPCONF_compilerIsMSVC)
    LIST(APPEND CMPCONF_cppOptions_common
         /bigobj              # increase number of addressable sections
         /diagnostics:column  # format of diagnostics message
         /EHsc                # exception handling: stack unwinding
         /fp:fast             # fast floating point calculation
         /Gd                  # cdecl calling convention
         /GS                  # buffers security check
         /MP                  # multi processor compilation
         /nologo              # suppress display of banner
         /permissive-         # set strict standard conformance
         /Zc:forScope         # standard conformance for scoping
         /Zc:inline           # remove unreferenced functions
         /Zc:preprocessor     # conforming preprocessor
         /Zc:wchar_t          # wchar is native
    )

    LIST(APPEND CMPCONF_cppOptions_release
         /Gw                  # global program optimization
         /O2                  # generate fast code
         /Qpar                # enables loop parallelization
    )

    LIST(APPEND CMPCONF_cppOptions_relWithDebInfo
         /Z7                  # debug information in file
         /Od                  # no optimization
    )

    LIST(APPEND CMPCONF_cppOptions_debug
         /Od                  # no optimization
         /Zi                  # debug information in database
    )

    SET(CMPCONF_cppLinkerOptions_common )
ELSE()
    LIST(APPEND CMPCONF_cppOptions_common
         -ffast-math                  # fast floating point calculation
         -fvisibility=hidden          # default symbol visibility is
                                      # hidden
         -fvisibility-inlines-hidden  # hide inline functions
         -O0                          # no optimization
         -Ofast                       # favors fast code
         -pedantic                    # set strict standard conformance
    )

    IF(MACOS)
        LIST(APPEND CMPCONF_cppOptions_common
             -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk)
    ENDIF(MACOS)

    LIST(APPEND CMPCONF_cppOptions_release
         -O3                  # extreme optimization
    )

    LIST(APPEND CMPCONF_cppOptions_relWithDebInfo
         -Og                  # debugging compatible optimization
         -g                   # debug information in object files
    )

    LIST(APPEND CMPCONF_cppOptions_debug
         -Og                  # debugging compatible optimization
         -g                   # debug information in object files
    )

    # warn about undefined symbols when linking
    IF(MACOS)
        # LIST(APPEND CMPCONF_cppLinkerOptions_common
        #      -Wl,-undefined,error)
    ELSE()
        # LIST(APPEND CMPCONF_cppLinkerOptions_common
        #      -Wl,-no-undefined)
    ENDIF(MACOS)           
ENDIF()

LIST(APPEND CMPCONF_cppWarningOptions
     ${CMPCONF_cppWarningOptions_common}
     $<$<CONFIG:Release>:${CMPCONF_cppWarningOptions_release}>)

SET(CMAKE_EXE_LINKER_FLAGS ${CMPCONF_cppLinkerOptions_common}
    CACHE STRING "" FORCE)
SET(CMAKE_MODULE_LINKER_FLAGS ${CMPCONF_cppLinkerOptions_common}
    CACHE STRING "" FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS ${CMPCONF_cppLinkerOptions_common}
    CACHE STRING "" FORCE)
SET(CMAKE_STATIC_LINKER_FLAGS ${CMPCONF_cppLinkerOptions_common}
    CACHE STRING "" FORCE)
