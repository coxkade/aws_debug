
cmake_minimum_required(VERSION 3.13)

project(kade_app)

set(AFR_BOARD espressif.esp32_devkitc CACHE INTERNAL "")

add_executable(kade_app main.c)
set(IDF_PROJECT_EXECUTABLE kade_app)

add_subdirectory(amazon-freertos)

target_include_directories( kade_app
    PRIVATE include
    PRIVATE amazon-freertos/demos/network_manager/
)

target_sources( kade_app
PRIVATE amazon-freertos/vendors/nordic/boards/nrf52840-dk/aws_tests/application_code/aws_ble_numericComparison.c
PRIVATE amazon-freertos/demos/demo_runner/aws_demo_network_addr.c
PRIVATE amazon-freertos/demos/demo_runner/aws_demo.c)

target_link_libraries(kade_app
PRIVATE AFR::common 
PRIVATE AFR::freertos
PRIVATE AFR::dev_mode_key_provisioning
PRIVATE AFR::mqtt
PRIVATE AFR::freertos_plus_tcp
PRIVATE AFR::wifi
PRIVATE AFR::utils
)

add_custom_command(
	OUTPUT kade_app_dissassembly.txt
	COMMAND ${CMAKE_OBJDUMP} "-dwCSl" "-Mintel" "--file-start-context" ${CMAKE_BINARY_DIR}/kade_app > kade_app_dissassembly.txt
	DEPENDS kade_app
)

add_custom_target( disassembly 
	DEPENDS kade_app_dissassembly.txt
)