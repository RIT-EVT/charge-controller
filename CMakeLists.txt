###############################################################################
# Top level CMakeList for building the EVT APM source code
###############################################################################
cmake_minimum_required(VERSION 3.15)

set(EVT_CORE_DIR      ${CMAKE_SOURCE_DIR}/libs/EVT-core)

option(TARGET_DEV "Target device" "STM32F302x8")
if(NOT TARGET_DEV)
    set(TARGET_DEV "STM32F302x8")
endif()

if(TARGET_DEV STREQUAL "STM32F302x8")
    add_compile_definitions(STM32F302x8)
    add_compile_definitions(STM32F3xx)
elseif(TARGET_DEV STREQUAL "STM32F334x8")
    add_compile_definitions(STM32F334x8)
    add_compile_definitions(STM32F3xx)
else()
    message(FATAL_ERROR "The target device is not supported")
endif()

add_compile_definitions(CANOPEN_QUEUE_SIZE=50)