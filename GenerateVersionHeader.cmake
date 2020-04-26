GET_FILENAME_COMPONENT(SRC_DIR ${SRC} DIRECTORY)

# Generate a git-describe-like version string from Mercurial repository tags
IF (HG_EXECUTABLE AND NOT DEFINED VAL_VERSION)
  EXECUTE_PROCESS(
      COMMAND ${HG_EXECUTABLE} log --rev . --template
      "{latesttag}{sub\('\^-0-.*', '', '-{latesttagdistance}-m{node|short}'\)}"
      WORKING_DIRECTORY ${SRC_DIR}
      OUTPUT_VARIABLE HG_REVISION
      RESULT_VARIABLE HG_LOG_ERROR_CODE
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Append "-dirty" if the working copy is not clean
  EXECUTE_PROCESS(
      COMMAND ${HG_EXECUTABLE} id --id
      WORKING_DIRECTORY ${SRC_DIR}
      OUTPUT_VARIABLE HG_ID
      RESULT_VARIABLE HG_ID_ERROR_CODE
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  # The hg id ends with '+' if there are uncommitted local changes
  IF (HG_ID MATCHES "\\+$")
    SET(HG_REVISION "${HG_REVISION}-dirty")
  ENDIF ()

  IF (NOT HG_LOG_ERROR_CODE AND NOT HG_ID_ERROR_CODE)
    SET(EMBRYO_VERSION ${HG_REVISION})
  ENDIF ()
ENDIF ()

# Generate a git-describe version string from Git repository tags
IF (GIT_EXECUTABLE AND NOT DEFINED EMBRYO_VERSION)
  EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --match "v*"
      WORKING_DIRECTORY ${SRC_DIR}
      OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
      RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  IF (NOT GIT_DESCRIBE_ERROR_CODE)
    SET(EMBRYO_VERSION ${GIT_DESCRIBE_VERSION})
  ENDIF ()
ENDIF ()

# Final fallback: Just use a bogus version string that is semantically older
# than anything else and spit out a warning to the developer.
IF (NOT DEFINED EMBRYO_VERSION)
  SET(EMBRYO_VERSION v0.0.0-unknown)
  MESSAGE(WARNING "Failed to determine EMBRYO_VERSION from repository tags. Using default version \"${EMBRYO_VERSION}\".")
ENDIF ()

CONFIGURE_FILE(${SRC} ${DST} @ONLY)