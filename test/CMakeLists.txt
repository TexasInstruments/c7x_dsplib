message(STATUS "                                                     ")
message(STATUS "-----------------------------------------------------")
message(STATUS "   Generating configs for testing ${KERNEL_NAME}     ")
message(STATUS "-----------------------------------------------------")
message(STATUS "                                                     ")

include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/gen_test_cases.cmake)

if(AUTO_TEST)
  # TODO: complete dependency that can help with regression testing the library
  # file(GLOB SUBDIRS "${CMAKE_CURRENT_LIST_DIR}/*") message("sub dirs are
  # ${SUBDIRS}")

  # message("kernels_list: ${KERNEL_LIST}") foreach (kernel ${KERNEL_LIST})
  # message("kernel name: ${kernel}") endforeach ()

  file(GLOB SUBDIRS "${CMAKE_CURRENT_LIST_DIR}/*")

  foreach(subdir ${SUBDIRS})
    if(IS_DIRECTORY ${subdir})
      get_filename_component(nameOnly ${subdir} NAME)

      if(NOT ${nameOnly} STREQUAL "DSPLIB_idat_gen")
        message(STATUS "Adding kernel name ${nameOnly}")
        add_subdirectory(${subdir})
      endif()

    endif()
  endforeach()
else()
  add_subdirectory(common)
  add_subdirectory(${KERNEL_NAME})
endif()
