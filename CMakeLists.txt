# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================


# ====================================================================================
# cmake version
cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2 CACHE STRING "Board type")

# include the sdk.cmake file
include(pico_sdk_import.cmake)

# give the project a name (anything you want)
project(fortuna-computer-base)

# initialize the sdk
pico_sdk_init()

#######

# name anything you want
add_executable(${CMAKE_PROJECT_NAME})

# must match with pio filename and executable name from above
pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/hsync.pio)
pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/vsync.pio)
pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/rgb.pio)

# must match with executable name and source file names
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
	#vga16_graphics_font.c
		main.cc
		vga/vga.cc
)

# must match with executable name
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
	pico_stdlib 
	pico_bootsel_via_double_reset 
	hardware_pio 
	hardware_dma 
	hardware_adc 
	hardware_sync
	hardware_irq
	pico_multicore
)

# must match with executable name
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

add_compile_options(-O3 )

add_custom_target(upload-remote
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/pico-pi-upload-client.sh $ENV{REMOTE_URL} rp2350 ${CMAKE_PROJECT_NAME}.elf
		DEPENDS ${CMAKE_PROJECT_NAME}
		COMMENT "Uploading remotely..."
		VERBATIM
)