cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE cubemx.cmake/arm-gcc.cmake)
include(cubemx.cmake/cubemx.cmake)

project(baremetal-tetris)

file(
    GLOB_RECURSE APP_SRC_FILES "app/*.c" "coop-sched/*.c"
)

add_executable(baremetal-tetris ${APP_SRC_FILES})

if(NOT BOARD)
    set(BOARD L433)
endif()
string(TOUPPER ${BOARD} BOARD)

message("Board selected: ${BOARD}")

set(L433_DIR "${CMAKE_CURRENT_LIST_DIR}/nucleo_l433rc_p")
set(L433_IOC "${L433_DIR}/nucleo_l433rc_p.ioc")

set(BLUEPILL_DIR "${CMAKE_CURRENT_LIST_DIR}/bluepill")
set(BLUEPILL_IOC "${BLUEPILL_DIR}/bluepill.ioc")
# For Blue Pill, these definitions have to be set explicitly
set(BLUEPILL_STARTUP "${BLUEPILL_DIR}/startup_stm32f103xb.s")
set(BLUEPILL_CDEF STM32F103xB)
set(BLUEPILL_PYOCD_OPT "-tstm32f103c8")

set(BOARD_DIR ${${BOARD}_DIR})
set(BOARD_IOC ${${BOARD}_IOC})
set(BOARD_STARTUP ${${BOARD}_STARTUP})
set(BOARD_CDEF ${${BOARD}_CDEF})
set(PYOCD_OPT ${${BOARD}_PYOCD_OPT})

if(NOT BOARD_DIR)
    message(FATAL_ERROR "Unknown board ${BOARD}")
endif()

if("${BOARD}" STREQUAL "BLUEPILL")
    # Add USB CDC stuff
    target_include_directories(baremetal-tetris PRIVATE
        "${BOARD_DIR}/USB_DEVICE/App"
        "${BOARD_DIR}/USB_DEVICE/Target"
        "${BOARD_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc"
        "${BOARD_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"
    )
    file(GLOB_RECURSE USB_CDC_SRC
        "${BOARD_DIR}/USB_DEVICE/*.c"
        "${BOARD_DIR}/Middlewares/*.c"
    )
    target_sources(baremetal-tetris PRIVATE ${USB_CDC_SRC})
endif()

cubemx_target(
    baremetal-tetris
    IOC "${BOARD_IOC}"
    CUBEMX_SOURCE_DIR "${BOARD_DIR}"
    STARTUP "${BOARD_STARTUP}"
)
target_compile_options(baremetal-tetris PRIVATE -Og -Wall -g -gdwarf-2)
target_include_directories(baremetal-tetris PUBLIC "${CMAKE_CURRENT_LIST_DIR}/app" "${CMAKE_CURRENT_LIST_DIR}/coop-sched")
target_compile_definitions(baremetal-tetris PRIVATE USE_FULL_LL_DRIVER ENABLE_STACK_WATERMARK ${BOARD_CDEF})
