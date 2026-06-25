# Doxygen

# look for Doxygen package
find_package(Doxygen)
# find_package(MathJax)

if(DOXYGEN_FOUND)
  # set input and output files
  set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/doxygen/doxyfile_user_guide)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

  # request to configure the file
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
  message(STATUS "Doxygen build started")

  # bad hack
  file(READ ${DOXYGEN_OUT} FILE_CONTENTS)
  string(REPLACE "./docs" "../docs" FILE_CONTENTS "${FILE_CONTENTS}")
  file(WRITE ${DOXYGEN_OUT} "${FILE_CONTENTS}")
  
  set(DOXYGEN_LOG ${CMAKE_CURRENT_BINARY_DIR}/doxygen/doxygen-console.log)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen)

  # Note: do not put "ALL" - this builds docs together with application EVERY
  # TIME!
  add_custom_target(
    user_guide
    COMMAND ${CMAKE_COMMAND} -E echo "Running Doxygen, logging to ${DOXYGEN_LOG}"
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT} > "${DOXYGEN_LOG}" 2>&1
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
else(DOXYGEN_FOUND)
  message(STATUS "Doxygen need to be installed to generate the doxygen documentation")
endif(DOXYGEN_FOUND)
