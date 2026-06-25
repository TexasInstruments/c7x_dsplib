## Call util function to create rules for building object libs for this kernel
if(NOT BUILD_LIB)
  set(extKernelName "DSPLIB_matTrans")
  add_subdirectory("../${extKernelName}" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
DSPLIB_create_object_lib_for_kernel()