cmake_minimum_required(VERSION 3.16)

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
set(L433_CONSOLE "USART2")

set(BLUEPILL_DIR "${CMAKE_CURRENT_LIST_DIR}/bluepill")
set(BLUEPILL_IOC "${BLUEPILL_DIR}/bluepill.ioc")
# For Blue Pill, these definitions have to be set explicitly
set(BLUEPILL_STARTUP "${BLUEPILL_DIR}/startup_stm32f103xb.s")
set(BLUEPILL_CDEF STM32F103xB)
set(BLUEPILL_PYOCD_OPT "-tstm32f103c8")
set(BLUEPILL_CONSOLE "USB")

set(F401_DIR "${CMAKE_CURRENT_LIST_DIR}/blackpill_f401")
set(F401_IOC "${F401_DIR}/blackpill_f401.ioc")
set(F401_CDEF STM32F401xC)
set(F401_STARTUP "${F401_DIR}/startup_stm32f401xc.s")
set(F401_CONSOLE "USB")

set(BOARD_DIR ${${BOARD}_DIR})
set(BOARD_IOC ${${BOARD}_IOC})
set(BOARD_STARTUP ${${BOARD}_STARTUP})
set(BOARD_CDEF ${${BOARD}_CDEF})
set(BOARD_CONSOLE ${${BOARD}_CONSOLE})
set(PYOCD_OPT ${${BOARD}_PYOCD_OPT})

if(NOT BOARD_DIR)
    message(FATAL_ERROR "Unknown board ${BOARD}")
endif()

if("${BOARD_CONSOLE}" STREQUAL "USB")
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
    list(APPEND BOARD_CDEF CONSOLE_USB)
else()
    list(APPEND BOARD_CDEF
        -DCONSOLE_UART=${BOARD_CONSOLE}
        -DCONSOLE_IRQ_HANDLER=${BOARD_CONSOLE}_IRQHandler
    )
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
