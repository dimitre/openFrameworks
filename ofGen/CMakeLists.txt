cmake_minimum_required(VERSION 3.18)
project(ofgen LANGUAGES CXX)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Use a modern C++20 build
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------
# Cache variables for precompiled library directories
# ------------------------------------------------------------------
set(YAMLCPP_ROOT "" CACHE PATH "Root directory for yaml-cpp (containing lib/ and include/)")
set(PUGIXML_ROOT "" CACHE PATH "Root directory for pugixml (containing lib/ and include/)")
set(NLOHMANN_JSON_ROOT "" CACHE PATH "Root directory for nlohmann_json (containing include/)")

# Alternative: specify lib and include separately
set(YAMLCPP_LIBRARY_DIR "" CACHE PATH "Directory containing yaml-cpp library files")
set(YAMLCPP_INCLUDE_DIR "" CACHE PATH "Directory containing yaml-cpp headers")

set(PUGIXML_LIBRARY_DIR "" CACHE PATH "Directory containing pugixml library files")
set(PUGIXML_INCLUDE_DIR "" CACHE PATH "Directory containing pugixml headers")

set(NLOHMANN_JSON_INCLUDE_DIR "" CACHE PATH "Directory containing nlohmann_json headers")

# ------------------------------------------------------------------
# Create executable
# ------------------------------------------------------------------
add_executable(ofgen
    src/main.cpp
    src/addons.cpp
    src/templates.cpp
    src/utils.cpp
    src/uuidxx/src/uuidxx.cpp
)

target_compile_definitions(ofgen PRIVATE YAML_CPP_STATIC_DEFINE)

target_include_directories(ofgen
    PRIVATE
        src
        src/uuidxx/src
)

# ------------------------------------------------------------------
# Helper function to find static library
# ------------------------------------------------------------------
function(find_static_lib OUTPUT_VAR LIB_NAME SEARCH_PATHS)
    # Common static library patterns
    set(PATTERNS
        ${LIB_NAME}.a
        lib${LIB_NAME}.a
        ${LIB_NAME}.lib
        lib${LIB_NAME}.lib
    )

    foreach(PATTERN ${PATTERNS})
        find_file(${OUTPUT_VAR}_TEMP
            NAMES ${PATTERN}
            PATHS ${SEARCH_PATHS}
            NO_DEFAULT_PATH
        )
        if(${OUTPUT_VAR}_TEMP)
            set(${OUTPUT_VAR} ${${OUTPUT_VAR}_TEMP} PARENT_SCOPE)
            unset(${OUTPUT_VAR}_TEMP CACHE)
            return()
        endif()
        unset(${OUTPUT_VAR}_TEMP CACHE)
    endforeach()

    message(FATAL_ERROR "Could not find static library ${LIB_NAME} in paths: ${SEARCH_PATHS}")
endfunction()

# ------------------------------------------------------------------
# Dependency handling - Precompiled libraries
# ------------------------------------------------------------------
message(STATUS "Using precompiled static libraries")

# yaml-cpp
if(YAMLCPP_ROOT)
    set(YAMLCPP_LIBRARY_DIR "${YAMLCPP_ROOT}/lib")
    set(YAMLCPP_INCLUDE_DIR "${YAMLCPP_ROOT}/include")
endif()

if(NOT YAMLCPP_LIBRARY_DIR OR NOT YAMLCPP_INCLUDE_DIR)
    message(FATAL_ERROR "Either YAMLCPP_ROOT or both YAMLCPP_LIBRARY_DIR and YAMLCPP_INCLUDE_DIR must be set")
endif()

find_static_lib(YAMLCPP_LIBRARY "yaml-cpp" "${YAMLCPP_LIBRARY_DIR}")

add_library(yaml-cpp STATIC IMPORTED)
set_target_properties(yaml-cpp PROPERTIES
    IMPORTED_LOCATION "${YAMLCPP_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${YAMLCPP_INCLUDE_DIR}"
)

message(STATUS "Found yaml-cpp: ${YAMLCPP_LIBRARY}")
message(STATUS "  Include dir: ${YAMLCPP_INCLUDE_DIR}")

# pugixml
if(PUGIXML_ROOT)
    set(PUGIXML_LIBRARY_DIR "${PUGIXML_ROOT}/lib")
    set(PUGIXML_INCLUDE_DIR "${PUGIXML_ROOT}/include")
endif()

if(NOT PUGIXML_LIBRARY_DIR OR NOT PUGIXML_INCLUDE_DIR)
    message(FATAL_ERROR "Either PUGIXML_ROOT or both PUGIXML_LIBRARY_DIR and PUGIXML_INCLUDE_DIR must be set")
endif()

find_static_lib(PUGIXML_LIBRARY "pugixml" "${PUGIXML_LIBRARY_DIR}")

add_library(pugixml STATIC IMPORTED)
set_target_properties(pugixml PROPERTIES
    IMPORTED_LOCATION "${PUGIXML_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${PUGIXML_INCLUDE_DIR}"
)

message(STATUS "Found pugixml: ${PUGIXML_LIBRARY}")
message(STATUS "  Include dir: ${PUGIXML_INCLUDE_DIR}")

# nlohmann_json (header-only)
if(NLOHMANN_JSON_ROOT)
    set(NLOHMANN_JSON_INCLUDE_DIR "${NLOHMANN_JSON_ROOT}/include")
endif()

if(NOT NLOHMANN_JSON_INCLUDE_DIR)
    message(FATAL_ERROR "Either NLOHMANN_JSON_ROOT or NLOHMANN_JSON_INCLUDE_DIR must be set")
endif()

add_library(nlohmann_json INTERFACE IMPORTED)
set_target_properties(nlohmann_json PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${NLOHMANN_JSON_INCLUDE_DIR}"
)

message(STATUS "Found nlohmann_json: ${NLOHMANN_JSON_INCLUDE_DIR}")

# Link libraries
target_link_libraries(ofgen PRIVATE
    yaml-cpp
    pugixml
    nlohmann_json
)

# ------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------
install(TARGETS ofgen
    RUNTIME DESTINATION bin
)

if(WIN32)
    install(CODE [[
        set(INST_PREFIX "${CMAKE_INSTALL_PREFIX}")
        set(PATH_TO_ADD "${INST_PREFIX}\\bin")
        execute_process(
            COMMAND cmd /C setx PATH "\"$ENV{PATH};${PATH_TO_ADD}\""
            RESULT_VARIABLE rc
            ERROR_VARIABLE err
        )
        if(NOT rc EQUAL 0)
            message(WARNING "Failed to update PATH: ${err}")
        else()
            message(STATUS "Updated user PATH with ${PATH_TO_ADD}")
        endif()
    ]])
endif()
