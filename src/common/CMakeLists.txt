# file(GLOB_RECURSE COMMON_FILES "*.cpp" "*.c")

string(TOLOWER ${DEVICE} DEVICE_DIR)
file(GLOB COMMON_C_FILES "*.c"
                         "c71/*.c"
		         "${DEVICE_DIR}/*.c")
file(GLOB COMMON_CPP_FILES "*.cpp"
                           "c71/*.cpp"
		           "${DEVICE_DIR}/*.cpp")
file(GLOB COMMON_H_FILES "*.h"
                         "c71/*.h"
		         "${DEVICE_DIR}/*.h")			 

# list (APPEND "${CMAKE_CURRENT_SOURCE_DIR}/*.c")		
if(TARGET_PLATFORM STREQUAL "PC")
  set_source_files_properties( ${COMMON_C_FILES} PROPERTIES LANGUAGE CXX )
endif()

list(APPEND COMMON_FILES ${COMMON_CPP_FILES}
                         ${COMMON_C_FILES}
			 ${COMMON_H_FILES})
		       
## message("common files are: ${COMMON_FILES}")

add_library(DSPLIB_common STATIC ${COMMON_FILES})

DSPLIB_set_lib_extension(DSPLIB_common)

target_include_directories (DSPLIB_common PUBLIC ../../src/common . ${C7x_INCLUDES})
