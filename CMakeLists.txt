
cmake_minimum_required(VERSION 3.16)

include(cubemx.cmake/cubemx.cmake)

project(baremetal-tetris)

file(
    GLOB_RECURSE APP_SRC_FILES "app/*.c" "coop-sched/*.c"
)

add_executable(baremetal-tetris ${APP_SRC_FILES})

set(L433_DIR "${CMAKE_CURRENT_LIST_DIR}/nucleo_l433rc_p")
cubemx_target(
    baremetal-tetris
    IOC "${L433_DIR}/nucleo_l433rc_p.ioc"
    CUBEMX_SOURCE_DIR "${L433_DIR}"
)
target_compile_options(baremetal-tetris PRIVATE -Og -Wall -g -gdwarf-2)
target_include_directories(baremetal-tetris PUBLIC "${CMAKE_CURRENT_LIST_DIR}/app" "${CMAKE_CURRENT_LIST_DIR}/coop-sched")
target_compile_definitions(baremetal-tetris PRIVATE USE_FULL_LL_DRIVER ENABLE_STACK_WATERMARK)
