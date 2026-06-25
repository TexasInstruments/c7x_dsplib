## Build the common library as it's needed by all kernels
add_subdirectory(common)



if(BUILD_LIB)

  add_library(DSPLIB STATIC)
  DSPLIB_set_lib_extension(DSPLIB)

  # add_subdirectory(DSPLIB_round)
  # add_subdirectory(DSPLIB_recip)

  ######################################################################################
  ## CMake people do not recommend this; not sure about the correct way, needs review ##
  ######################################################################################

  file(GLOB SUBDIRS "${CMAKE_CURRENT_LIST_DIR}/*")
  # message("sub dirs are ${SUBDIRS}")
  
  foreach(subdir ${SUBDIRS})
    if (IS_DIRECTORY  ${subdir})
      # message("Found directory ${subdir}")

      get_filename_component(nameOnly ${subdir} NAME)

      if (NOT ${nameOnly} STREQUAL "common")

	# message ("Adding kernel name ${nameOnly}")

	add_subdirectory(${subdir})
	target_link_libraries(DSPLIB "${nameOnly}_obj")

      endif()
    endif ()
  endforeach()

  # get_target_property(OUT DSPLIB LINK_LIBRARIES)
  # message("Linked objs are ${OUT}")

elseif (BUILD_TEST)

  add_subdirectory(${KERNEL_NAME})

endif()  

