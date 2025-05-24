cmake_minimum_required(VERSION 3.13)
string(TIMESTAMP YEAR "%Y")

#
# configuration
#

set(PICO_BOARD pico2 CACHE STRING "Board type")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(pico_sdk_import.cmake)

#
# project
#

project(fortuna-computer-base VERSION 0.3.0)
pico_sdk_init()

#
# sources
#

add_subdirectory(contrib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src)

add_executable(${CMAKE_PROJECT_NAME}
		demo.cc
		firmware/vga/vga.hh
		firmware/vga/vga.cc
		firmware/vga/fb.cc
		firmware/vga/fb.hh
		firmware/vga/text.cc
		firmware/vga/text.hh
		firmware/vga/font.hh
		firmware/usb/usb.cc
		firmware/usb/usb.hh
		firmware/usb/keyboard.cc
		firmware/usb/keyboard.hh
		firmware/fortuna/fortuna.cc
		firmware/fortuna/fortuna.hh
		firmware/sdcard/hw_config.c
		firmware/user/user.cc
		firmware/user/user.hh
		firmware/rtc/rtc.cc
		firmware/rtc/rtc.hh)

pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/firmware/vga/pio/hsync.pio)
pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/firmware/vga/pio/vsync.pio)
pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/firmware/vga/pio/rgb.pio)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/firmware/fortuna ${CMAKE_SOURCE_DIR}/firmware/usb ${CMAKE_SOURCE_DIR}/firmware/sdcard)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE -DPROJECT_VERSION=\"${CMAKE_PROJECT_VERSION}\" -DYEAR=\"${YEAR}\")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
	pico_stdlib 
	pico_bootsel_via_double_reset
	hardware_pio 
	hardware_dma
	hardware_irq
	hardware_i2c
	pico_aon_timer
	pico_multicore
	tinyusb_host
	tinyusb_board
	no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

# must match with executable name
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

add_compile_options(-O3)

# upload remotely - see https://github.com/andrenho/pico-pi-upload-server

add_custom_target(upload-remote
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/pico-pi-upload-client.sh $ENV{REMOTE_URL} rp2350 ${CMAKE_PROJECT_NAME}.elf
		DEPENDS ${CMAKE_PROJECT_NAME}
		COMMENT "Uploading remotely..."
		VERBATIM
)