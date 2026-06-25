# set(CMAKE_C_COMPILER gcc-11)
set(CMAKE_CXX_COMPILER g++-11)

# set(CMAKE_AR                        gcc-ar-11) set(CMAKE_ASM_COMPILER as-11)
# set(CMAKE_LINKER                    g++-11) set(CMAKE_OBJCOPY g++-11)
# set(CMAKE_CXX_LINK_EXECUTABLE       ${CMAKE_CXX_COMPILER})
# set(CMAKE_C_LINK_EXECUTABLE         ${CMAKE_CXX_COMPILER})

# TI CGT Package has different naming scheme for C7524
set(DEVICE_C7524 "C7524-MMA2_256")

if(DEVICE STREQUAL "C7524")
  set(C7x_LIBS "$ENV{CGT7X_ROOT}/host_emulation/lib${DEVICE_C7524}-host-emulation.a")
else()

  set(C7x_LIBS "$ENV{CGT7X_ROOT}/host_emulation/lib${DEVICE}-host-emulation.a")
endif()

# message("This is the CXX compiler: ${CMAKE_CXX_COMPILER}")
