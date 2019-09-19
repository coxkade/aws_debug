cmake_minimum_required(VERSION 3.13)

#Function that creates and executable from source
macro(esp_override_add_executable ex)
    unset(ESP_OVERRIDE_EX_NAME)
    unset(ESP_OVERRIDE_SORCES)
    unset(ESP_OVERRIDE_INCLUDES)
    unset(ESP_OVERRIDE_LIBS)
    unset(IDF_PROJECT_EXECUTABLE)

    set(ESP_OVERRIDE_EX_NAME ${ex})
endmacro()

#Function that adds to the source list for our esp target
macro(esp_override_target_add_source source)
    get_filename_component(abs_path ${source} ABSOLUTE)
    list(APPEND ESP_OVERRIDE_SORCES ${abs_path})
endmacro()

#function that adds a path to the target
macro(esp_override_target_add_include path)
    get_filename_component(abs_path ${path} ABSOLUTE)
    list(APPEND ESP_OVERRIDE_INCLUDES ${abs_path})
endmacro()

#function that adds the library to the target
macro(esp_override_target_link_library lib)
    list(APPEND ESP_OVERRIDE_LIBS ${lib})
endmacro()

#function that prints the override variables
function(esp_override_print_vars)
    message(STATUS "ESP_OVERRIDE_EX_NAME: ${ESP_OVERRIDE_EX_NAME}")
    message(STATUS "ESP_OVERRIDE_SORCES: ${ESP_OVERRIDE_SORCES}")
    message(STATUS "ESP_OVERRIDE_INCLUDES: ${ESP_OVERRIDE_INCLUDES}")
    message(STATUS "ESP_OVERRIDE_LIBS: ${ESP_OVERRIDE_LIBS}")
    message(STATUS "IDF_PROJECT_EXECUTABLE: ${IDF_PROJECT_EXECUTABLE}")
endfunction()