## Call util function to create rules for building object libs for this kernel
DSPLIB_create_object_lib_for_kernel()

if(NOT BUILD_LIB)

  set(extKernelName "DSPLIB_matTrans")
  add_subdirectory("../${extKernelName}" "${CMAKE_SOURCE_DIR}/bin/Release/${extKernelName}")

  # Add one more directory
  set(extKernelName "DSPLIB_matMul")
  add_subdirectory("../${extKernelName}" "${CMAKE_SOURCE_DIR}/bin/Release/${extKernelName}")
endif()