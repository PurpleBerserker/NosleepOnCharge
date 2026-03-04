cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

add_definitions(-DLOG_DEBUG)

project(noSleepOnCharge)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O3 -std=gnu99")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")

include_directories(
)

link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(noSleepOnCharge.elf
  main.c
  logging/log.c
)

target_link_libraries(${PROJECT_NAME}.elf
  SceLibKernel_stub
  ScePowerForDriver_stub
  SceDebugForDriver_stub
  SceThreadmgrForDriver_stub
  SceIofilemgrForDriver_stub
  SceKernelSuspendForDriver_stub
  SceLibc_stub
  gcc
)

set_target_properties(${PROJECT_NAME}.elf
	PROPERTIES LINK_FLAGS "-nostdlib"
	COMPILE_FLAGS "-D__VITA_KERNEL__"
)

add_custom_target(${PROJECT_NAME}.skprx ALL
	COMMAND vita-elf-create -e ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.yml ${PROJECT_NAME}.elf ${PROJECT_NAME}.velf ${CMAKE_SOURCE_DIR}/taihen.json
	COMMAND vita-make-fself -c ${PROJECT_NAME}.velf ${PROJECT_NAME}.skprx
)
add_dependencies(${PROJECT_NAME}.skprx ${PROJECT_NAME}.elf)

add_custom_target(send
	COMMAND curl -T ${PROJECT_NAME}.skprx ftp://$(PSVITAIP):1337/ux0:/tai/
	DEPENDS ${PROJECT_NAME}.skprx
)
