# Doxygen + Sphinx documentation build
#
# Target: user_guide
#   Step 1 -- Run Doxygen in XML-only mode to produce <build>/docs/xml/
#   Step 2 -- Run sphinx-build to produce <build>/docs/user_guide/index.html
#
# The conf.py in docs/sphinx/ reads the XML path from the environment variable
# DSPLIB_DOXYGEN_XML, which this target sets to the absolute XML output dir
# before invoking sphinx-build.

# look for Doxygen package
find_package(Doxygen)

# look for sphinx-build
find_program(SPHINX_BUILD_EXECUTABLE NAMES sphinx-build)

if(DOXYGEN_FOUND AND SPHINX_BUILD_EXECUTABLE)

  # ------------------------------------------------------------------
  # CMake variables substituted into doxyfile_sphinx via configure_file
  #
  #   DOXYGEN_XML_OUT_PARENT  -- parent of the xml/ subdir
  #                              => ${CMAKE_CURRENT_BINARY_DIR}/docs
  #                              => XML lands in ${CMAKE_CURRENT_BINARY_DIR}/docs/xml
  #
  #   DOXYGEN_SRC_ROOT        -- absolute source-tree root (replaces "../" tricks)
  #                              => ${CMAKE_SOURCE_DIR}
  #
  #   DOXYGEN_SRC_DOCS        -- absolute path to source-tree docs/
  #                              => ${CMAKE_SOURCE_DIR}/docs
  #                              Replaces the legacy "./docs" -> "../docs" string-
  #                              replace hack that was needed because relative paths
  #                              in the old doxyfile_user_guide were interpreted
  #                              relative to WORKING_DIRECTORY (the build dir), so
  #                              "./docs/doxygen/..." pointed into the build tree
  #                              instead of the source tree.
  # ------------------------------------------------------------------
  set(DOXYGEN_XML_OUT_PARENT "${CMAKE_CURRENT_BINARY_DIR}/docs")
  set(DOXYGEN_SRC_ROOT       "${CMAKE_SOURCE_DIR}")
  set(DOXYGEN_SRC_DOCS       "${CMAKE_SOURCE_DIR}/docs")

  set(DOXYGEN_SPHINX_IN  ${CMAKE_SOURCE_DIR}/docs/doxygen/doxyfile_sphinx)
  set(DOXYGEN_SPHINX_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.sphinx.out)

  configure_file(${DOXYGEN_SPHINX_IN} ${DOXYGEN_SPHINX_OUT} @ONLY)
  message(STATUS "Doxygen+Sphinx build configured")

  # Ensure the log directory exists at configure time so the COMMAND
  # redirect does not fail on a clean build.
  set(DOXYGEN_LOG_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
  file(MAKE_DIRECTORY ${DOXYGEN_LOG_DIR})
  set(DOXYGEN_LOG ${DOXYGEN_LOG_DIR}/doxygen-console.log)

  # Sphinx HTML output directory -- MUST match contract:
  #   <build>/docs/user_guide/index.html
  set(SPHINX_HTML_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/user_guide)

  # Sphinx source directory (contains conf.py and index.rst)
  set(SPHINX_SOURCE_DIR ${CMAKE_SOURCE_DIR}/docs/sphinx)

  # Absolute XML dir passed to sphinx-build via env var DSPLIB_DOXYGEN_XML
  set(DOXYGEN_XML_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/xml)

  # Note: do not put "ALL" -- this builds docs together with the application
  # EVERY TIME which is undesirable for incremental builds.
  add_custom_target(
    user_guide
    COMMAND ${CMAKE_COMMAND} -E echo
            "Running Doxygen (XML), logging to ${DOXYGEN_LOG}"
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_SPHINX_OUT}
              > "${DOXYGEN_LOG}" 2>&1
    COMMAND ${CMAKE_COMMAND} -E echo
            "Running sphinx-build -> ${SPHINX_HTML_OUT}"
    COMMAND ${CMAKE_COMMAND} -E env
              DSPLIB_DOXYGEN_XML=${DOXYGEN_XML_DIR}
              ${SPHINX_BUILD_EXECUTABLE}
              -b html
              -W --keep-going
              ${SPHINX_SOURCE_DIR}
              ${SPHINX_HTML_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating Sphinx (TI-branded) API docs"
    VERBATIM)

else()

  if(NOT DOXYGEN_FOUND)
    message(STATUS
      "Doxygen not found -- install Doxygen to enable the user_guide target")
  endif()
  if(NOT SPHINX_BUILD_EXECUTABLE)
    message(STATUS
      "sphinx-build not found -- install Sphinx to enable the user_guide target."
      "  Hint: pip install -r docs/sphinx/requirements.txt")
  endif()

endif()
