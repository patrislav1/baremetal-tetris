
cmake_minimum_required(VERSION 3.16)

include(cubemx.cmake/cubemx.cmake)

project(coop-test)

file(
    GLOB_RECURSE APP_SRC_FILES "app/*.c" "coop-sched/*.c"
)

add_executable(coop-test ${APP_SRC_FILES})

cubemx_target(
    TARGET coop-test
    IOC "${CMAKE_CURRENT_LIST_DIR}/coop_test.ioc"
)
target_compile_options(coop-test PRIVATE -Og -Wall -g -gdwarf-2)
target_include_directories(coop-test PUBLIC "${CMAKE_CURRENT_LIST_DIR}/app" "${CMAKE_CURRENT_LIST_DIR}/coop-sched")
target_compile_definitions(coop-test PRIVATE USE_FULL_LL_DRIVER ENABLE_STACK_WATERMARK)
