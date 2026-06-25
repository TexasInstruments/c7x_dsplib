function(dsplib_gen_test_cases kernelName)

  add_custom_command(
    OUTPUT "${CMAKE_SOURCE_DIR}/test/${kernelName}/${kernelName}_idat.c"
    COMMAND python3
            ${CMAKE_SOURCE_DIR}/test/DSPLIB_idat_gen/${kernelName}/gen_data.py
    DEPENDS
      ${CMAKE_SOURCE_DIR}/test/DSPLIB_idat_gen/${kernelName}/gen_data.py
      ${CMAKE_SOURCE_DIR}/test/DSPLIB_idat_gen/${kernelName}/test_cases_list.csv
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test/DSPLIB_idat_gen/${kernelName}")

  add_custom_target(
    gen_test_case_headers
    DEPENDS "${CMAKE_SOURCE_DIR}/test/${kernelName}/${kernelName}_idat.c")

endfunction()

add_custom_target(
  gen_all_test_case_headers
  COMMAND ${CMAKE_SOURCE_DIR}/cmake/gen_all_test_case_headers.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake/)
