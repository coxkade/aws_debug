
cmake_minimum_required(VERSION 3.13)
include(esp_cmake_helper.cmake)

set(CMAKE_BUILD_TYPE Debug)
set(AFR_BOARD espressif.esp32_devkitc CACHE INTERNAL "")

get_filename_component(abs_path include ABSOLUTE)
set(AWS_DEMOS_CONFIG_FILES_LOCATION_OVERRIDE ${abs_path})

project(kade_app)
#create the executable
esp_override_add_executable(kade_app)
#add the sources
esp_override_target_add_source(main.c)
esp_override_target_add_source(amazon-freertos/demos/wifi_provisioning/aws_wifi_connect_task.c)
#add the includes
esp_override_target_add_include(include)
#add the libraries
esp_override_target_link_library(AFR::wifi)
esp_override_target_link_library(AFR::utils)
esp_override_target_link_library(AFR::ble)

esp_override_print_vars()

add_subdirectory(amazon-freertos)

list(TRANSFORM AFR_DEMOS_ENABLED PREPEND "AFR::" OUTPUT_VARIABLE demos_list)
message(STATUS "demos_list: ${demos_list}")
target_link_libraries(kade_app PRIVATE ${demos_list})

add_custom_command(
	OUTPUT kade_app_dissassembly.txt
	COMMAND ${CMAKE_OBJDUMP} "-dwCSl" "-Mintel" "--file-start-context" ${CMAKE_BINARY_DIR}/kade_app > kade_app_dissassembly.txt
	DEPENDS kade_app
)

add_custom_target( disassembly 
	DEPENDS kade_app_dissassembly.txt
)

esp_override_print_vars()