cmake_minimum_required(VERSION 3.13)
string(TIMESTAMP YEAR "%Y")

#
# configuration
#

set(PICO_BOARD pico2 CACHE STRING "Board type" PARENT_SCOPE)

set(CMAKE_CXX_STANDARD 20 PARENT_SCOPE)
set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)

include(../pico_sdk_import.cmake)
pico_sdk_init()

#
# project
#

project(fortuna-io-board VERSION 2.0)

add_library(${PROJECT_NAME} INTERFACE
		vga/sprite.hh
		vga/color.hh)

#
# sources
#

add_subdirectory(contrib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src)

target_sources(${PROJECT_NAME} INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/vga/color.hh
		${CMAKE_CURRENT_LIST_DIR}/vga/vga.hh
		${CMAKE_CURRENT_LIST_DIR}/vga/vga.cc
		${CMAKE_CURRENT_LIST_DIR}/vga/fb.cc
		${CMAKE_CURRENT_LIST_DIR}/vga/fb.hh
		${CMAKE_CURRENT_LIST_DIR}/vga/text.cc
		${CMAKE_CURRENT_LIST_DIR}/vga/text.hh
		${CMAKE_CURRENT_LIST_DIR}/vga/font.hh
		${CMAKE_CURRENT_LIST_DIR}/vga/sprite.hh
		${CMAKE_CURRENT_LIST_DIR}/usb/usb.cc
		${CMAKE_CURRENT_LIST_DIR}/usb/usb.hh
		${CMAKE_CURRENT_LIST_DIR}/usb/keyboard.cc
		${CMAKE_CURRENT_LIST_DIR}/usb/keyboard.hh
		${CMAKE_CURRENT_LIST_DIR}/fortuna/fortuna.cc
		${CMAKE_CURRENT_LIST_DIR}/fortuna/fortuna.hh
		${CMAKE_CURRENT_LIST_DIR}/audio/audio.cc
		${CMAKE_CURRENT_LIST_DIR}/audio/audio.hh
		${CMAKE_CURRENT_LIST_DIR}/external/external.cc
		${CMAKE_CURRENT_LIST_DIR}/external/external.hh
		${CMAKE_CURRENT_LIST_DIR}/sdcard/hw_config.c
		${CMAKE_CURRENT_LIST_DIR}/user/user.cc
		${CMAKE_CURRENT_LIST_DIR}/user/user.hh
		${CMAKE_CURRENT_LIST_DIR}/rtc/rtc.cc
		${CMAKE_CURRENT_LIST_DIR}/rtc/rtc.hh)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/hsync.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/vsync.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/rgb640.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/rgb320.pio)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/fortuna ${CMAKE_CURRENT_LIST_DIR}/usb ${CMAKE_CURRENT_LIST_DIR}/sdcard)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DPROJECT_VERSION=\"${PROJECT_VERSION}\" -DYEAR=\"${YEAR}\")

target_link_libraries(${PROJECT_NAME} INTERFACE
	pico_stdlib 
	pico_bootsel_via_double_reset
	hardware_pio 
	hardware_dma
	hardware_irq
	hardware_i2c
	hardware_pwm
	pico_aon_timer
	pico_multicore
	tinyusb_host
	tinyusb_board
	no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)
