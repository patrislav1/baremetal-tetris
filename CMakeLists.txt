
cmake_minimum_required(VERSION 3.16)

set(CMX_IOC "${CMAKE_CURRENT_LIST_DIR}/coop_test.ioc")

include(cmake/cubemx.cmake)

project(${CMX_PROJ})

add_executable(${CMX_PROJ} app/app.c app/ringbuf.c app/uart.c)

cubemx_target(${CMX_PROJ})
target_compile_options(${CMX_PROJ} PRIVATE -Og -Wall -g -gdwarf-2)
target_include_directories(${CMX_PROJ} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/app")
target_compile_definitions(${CMX_PROJ} PRIVATE USE_FULL_LL_DRIVER)
