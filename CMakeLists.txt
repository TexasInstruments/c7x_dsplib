cmake_minimum_required(VERSION 3.16.0)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/colorized_messages.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/clang-format.cmake)
# include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindMathJax.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/doxygen.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/coverage.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/username.cmake)

# Build options
option(BUILD_LIB "Build only DSPLIB and DSPLIB_test_common libraries (no test executables)" OFF)

# Check if CGT7X_ROOT env variable is set message("CGT7X_ROOT:$ENV{CGT7X_ROOT}")
if(NOT DEFINED ENV{CGT7X_ROOT})
	resolve_linux_username(CURRENT_USER)
	if (CURRENT_USER STREQUAL "")
		message(FATAL_ERROR "User name not found")
	endif()
	set(HOME "/home/${CURRENT_USER}")
	set(ENV{CGT_C7X_VERSION} "5.0.0.LTS")
	set(_TOOLS_PATH "${HOME}/ti")
	set(_CGT7X_ROOT "${_TOOLS_PATH}/ti-cgt-c7000_$ENV{CGT_C7X_VERSION}")
	set(ENV{TOOLS_PATH} "${_TOOLS_PATH}")
	set(ENV{CGT7X_ROOT} "${_CGT7X_ROOT}")
	set(ENV{PATH} "$ENV{CGT7X_ROOT}/bin:$ENV{PATH}")
	message(
	STATUS
	"${Green} CGT7X_ROOT env variable is set.")
else()
	message(STATUS "HOME environment variable is already set: $ENV{CGT7X_ROOT}")
endif()

# Set release as default build type
if(NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        "Release"
        CACHE STRING "" FORCE)
  endif()
endif()

# CMake custom util functions
include(${CMAKE_CURRENT_LIST_DIR}/cmake/utils.cmake)
dsplib_print_build_details()

if(TARGET_PLATFORM STREQUAL "PC")
  set(CMAKE_EXPORT_COMPILE_COMMANDS
      ON
      CACHE INTERNAL "")

endif()

set(CMAKE_CXX_STANDARD 14)

# ##############################################################################
# Pick the correct tool chain file of HE vs C7x ##
# ##############################################################################

# TI CGT package has different naming convention for C7524
set(DEVICE_C7524 "C7524-MMA2_256")

if(TARGET_PLATFORM STREQUAL "PC")

  if(DEVICE STREQUAL "C7524")
    set(C7x_INCLUDES "$ENV{CGT7X_ROOT}/host_emulation/include/${DEVICE_C7524}")
  else()
    set(C7x_INCLUDES "$ENV{CGT7X_ROOT}/host_emulation/include/${DEVICE}")
  endif()

  if(CMAKE_HOST_WIN32)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/msvc.cmake")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

    # Place all bins static libs in known place
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")
  else()
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/gcc.cmake")
    # Place all bins static libs in known place
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
  endif()

else()

  # Place all bins static libs in known place
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/c7x.cmake")
  include_directories(BEFORE SYSTEM $ENV{CGT7X_ROOT}/include)

endif()

# Set compilation flags for release and debug based on TARGET_PLATFORM
dsplib_compile_options()
dsplib_include_xlib_headers()

project(DSPLIB)

# Place all bins static libs in known place
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "") # works
# ##############################################################################
# Create dependency based on testing a kernel or running the example for a given
# kernel #
# ##############################################################################

if(BUILD_TEST)

  add_subdirectory(test)

elseif(AUTO_TEST)

  set(BUILD_LIB 1)
  add_subdirectory(test)

elseif(BUILD_EXAMPLE)

  set(BUILD_LIB 1)
  add_subdirectory(examples)

  # if (TARGET_PLATFORM STREQUAL "DSP") # set(CMAKE_EXE_LINKER_FLAGS
  # "--library=${CMAKE_CURRENT_LIST_DIR}/build/bin/libDSPLIB.a
  # ${CMAKE_EXE_LINKER_FLAGS}") # message("This is the updated
  # CMAKE_EXE_LINKER_FLAGS:${CMAKE_EXE_LINKER_FLAGS}") endif()
  
  elseif(BUILD_LIB)
  # Build only TISP library and TISP_test_common library (no test executables)
  add_subdirectory(test/common)

endif()

# Rules for building src directory
add_subdirectory(src bin)
