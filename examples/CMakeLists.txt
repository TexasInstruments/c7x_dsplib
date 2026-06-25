message(STATUS "------------------------------------------")
message(STATUS "   Building example for ${KERNEL_NAME}    ")
message(STATUS  "------------------------------------------")

include(${CMAKE_SOURCE_DIR}/cmake/examples.cmake)
add_subdirectory(${KERNEL_NAME})
