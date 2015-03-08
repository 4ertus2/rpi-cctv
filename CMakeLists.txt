cmake_minimum_required(VERSION 2.8)

project(rpi_cctv)

#
#cmake	-DCMAKE_TOOLCHAIN_FILE=../arm-linux-gnueabihf.cmake
#		-DRPI_VC_ROOT=$HOME/rpi/userland/opt/vc ..

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
   message(SEND_ERROR "CMAKE_TOOLCHAIN_FILE not defined")
endif()

if(NOT DEFINED RPI_VC_ROOT)
	message(SEND_ERROR "RPI_VC_ROOT not defined")
endif()

# avoids annoying and pointless warnings from gcc
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -U_FORTIFY_SOURCE")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -c")

set(SHARED "SHARED")

# All linux systems have sbrk()
add_definitions(-D_HAVE_SBRK)

# pull in declarations of lseek64 and friends
add_definitions(-D_LARGEFILE64_SOURCE)


# Global compiler flags
if(CMAKE_COMPILER_IS_GNUCC)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-multichar -Wall")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-multichar -Wall")
endif()

include_directories(
    ${RPI_VC_ROOT}/include
    ${RPI_VC_ROOT}/include/IL
    ${RPI_VC_ROOT}/include/interface/vcos/pthreads
    ${RPI_VC_ROOT}/include/interface/vmcs_host/linux
)

add_definitions(
    -D_REENTRANT
    -DOMX -DOMX_SKIP64BIT
    -DUSE_VCHIQ_ARM -DVCHI_BULK_ALIGN=1 -DVCHI_BULK_GRANULARITY=1
    -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
)

link_directories(
    ${RPI_VC_ROOT}/lib
)

set(LIBS openmaxil vcos bcm_host pthread)

#add_executable(rpi-omax-encode rpi-camera-encode.c)
#target_link_libraries(rpi-omax-encode ${LIBS})

add_executable(rpi-omax rpi-omax.cpp)
target_link_libraries(rpi-omax ${LIBS})
