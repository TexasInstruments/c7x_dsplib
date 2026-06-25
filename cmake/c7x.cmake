if (CMAKE_HOST_WIN32)
  set (SUFFIX .exe)
else()
  set (SUFFIX "")
endif()

# Without that flag CMake is not able to pass test compilation check
set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

set(CMAKE_AR                        $ENV{CGT7X_ROOT}/bin/ar7x${SUFFIX})
set(CMAKE_ASM_COMPILER              $ENV{CGT7X_ROOT}/bin/cl7x${SUFFIX})
set(CMAKE_C_COMPILER                $ENV{CGT7X_ROOT}/bin/cl7x${SUFFIX})
set(CMAKE_CXX_COMPILER              $ENV{CGT7X_ROOT}/bin/cl7x${SUFFIX})
set(CMAKE_LINKER                    $ENV{CGT7X_ROOT}/bin/cl7x${SUFFIX})
set(CMAKE_OBJCOPY                   $ENV{CGT7X_ROOT}/bin/cl7x${SUFFIX})
set(CMAKE_CXX_LINK_EXECUTABLE       ${CMAKE_CXX_COMPILER})

# set(CMAKE_C_FLAGS                   "--silicon_version=7504")
# set(CMAKE_CXX_FLAGS                 "--silicon_version=7504")

set(C7x_INCLUDES $ENV{CGT7X_ROOT}/include)
# set(CMAKE_EXE_LINKER_FLAGS    "--library=${CMAKE_LINKER} --ram_model --search_path=$ENV{CGT7X_ROOT}/lib")


# set(CMAKE_EXE_LINKER_FLAGS " ${_LINKER_CPU_FLAGS} -Wl,-T ${LINKER_FILE} -Wl,--gc-sections")
# set(CMAKE_C_FLAGS_DEBUG             "-Os -g" CACHE INTERNAL "")
# set(CMAKE_C_FLAGS_RELEASE           "-Os -DNDEBUG" CACHE INTERNAL "")
# set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "")
# set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "")

# message("This is the CXX compiler: ${CMAKE_CXX_COMPILER}")
