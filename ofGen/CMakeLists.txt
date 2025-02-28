cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 20)
project(ofGen LANGUAGES CXX)
find_package(yaml-cpp)

# add_subdirectory (src)
# add_subdirectory (src/uuidxx/src)


add_executable(ofGen src/main.cpp src/addons.cpp src/templates.cpp src/utils.cpp src/uuidxx/src/uuidxx.cpp)
# target_include_directories(ofGen PUBLIC src/uuidxx/src)
target_include_directories(
    ofGen
    PUBLIC src
    PUBLIC src/uuidxx/src
)

# find_package(PkgConfig)
find_package(yaml-cpp)
include_directories(${YAML_CPP_INCLUDE_DIR})
message(STATUS "YAML CMAKE FILE: ${yaml-cpp_CONFIG}")
message(STATUS "YAML_CPP_INCLUDE_DIR: [${YAML_CPP_INCLUDE_DIR}]")
message(STATUS "YAML_CPP_LIBRARY_DIR: [${YAML_CPP_LIBRARY_DIR}]")
message(STATUS "YAML_CPP_SHARED_LIBS_BUILT: [${YAML_CPP_SHARED_LIBS_BUILT}]")
message(STATUS "YAML_CPP_LIBRARIES: [${YAML_CPP_LIBRARIES}]")

#  YAML_CPP_INCLUDE_DIR       - include directory
#  YAML_CPP_LIBRARY_DIR       - directory containing libraries
#  YAML_CPP_SHARED_LIBS_BUILT - whether we have built shared libraries or not
#  YAML_CPP_LIBRARIES         - libraries to link against

include_directories(${YAML_INCLUDE_DIRS})
# target_link_libraries(ofGen ${YAML_LIBRARIES})
target_link_libraries(ofGen ${YAML_CPP_LIBRARIES})
target_include_directories(ofGen PUBLIC ${YAML_CPP_INCLUDE_DIR})
target_compile_options(ofGen PUBLIC ${YAML_CFLAGS_OTHER})

#find_package(PkgConfig QUIET)
find_package(nlohmann_json)
#pkg_check_modules(JSON "nlohmann_json")
message(STATUS "JSON: [${JSON}]")


target_link_libraries(ofGen ${JSON_LIBRARIES})
target_include_directories(ofGen PUBLIC ${JSON_INCLUDE_DIRS})
target_compile_options(ofGen PUBLIC ${JSON_CFLAGS_OTHER})
