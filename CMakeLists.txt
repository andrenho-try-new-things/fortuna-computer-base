cmake_minimum_required(VERSION 3.13)

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

project(fortuna-computer-base VERSION 0.9.1)
pico_sdk_init()

#
# sources
#

add_executable(${CMAKE_PROJECT_NAME}
		demo.cc
		vga/vga.hh
		vga/vga.cc
		vga/fb.cc
		vga/fb.hh
		vga/text.cc
		vga/text.hh
		vga/font.hh
		usb/usb.cc
		usb/usb.hh
		usb/keyboard.cc
		usb/keyboard.hh
		fortuna/fortuna.cc
		fortuna/fortuna.hh)

pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/hsync.pio)
pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/vsync.pio)
pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/rgb.pio)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/usb)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE -DPROJECT_VERSION=\"${CMAKE_PROJECT_VERSION}\")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
	pico_stdlib 
	pico_bootsel_via_double_reset
	hardware_pio 
	hardware_dma 
	# hardware_adc
	# hardware_sync
	# hardware_irq
	pico_multicore
	tinyusb_host
	tinyusb_board
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