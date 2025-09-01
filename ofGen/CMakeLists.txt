cmake_minimum_required(VERSION 3.18)
project(ofgen LANGUAGES CXX)

# Use a modern C++20 build
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure pkg‑config is available
find_package(PkgConfig REQUIRED)

# ------------------------------------------------------------------
# Find third‑party libraries (pkg‑config is the simplest, but you
# can also swap to find_package() if you prefer)
# ------------------------------------------------------------------
pkg_check_modules(YAML REQUIRED yaml-cpp)
pkg_check_modules(PUGIXML REQUIRED pugixml)
pkg_check_modules(JSON REQUIRED nlohmann_json)

# ------------------------------------------------------------------
# Add the executable
# ------------------------------------------------------------------
add_executable(ofgen
    src/main.cpp
    src/addons.cpp
    src/templates.cpp
    src/utils.cpp
    src/uuidxx/src/uuidxx.cpp
)

target_include_directories(ofgen
    PRIVATE
        ${YAML_INCLUDE_DIRS}
        ${PUGIXML_INCLUDE_DIRS}
        ${JSON_INCLUDE_DIRS}
        src
        src/uuidxx/src
)

link_directories(${YAML_LIBRARY_DIRS})          # Adds -L… to the link command

target_link_directories(
       ofgen PRIVATE
       ${YAML_LIBRARY_DIRS}   # expands to /opt/homebrew/Cellar/yaml-cpp/0.8.0/lib
       ${PUGIXML_LIBRARY_DIRS}   # expands to /opt/homebrew/Cellar/yaml-cpp/0.8.0/lib
   )

target_link_libraries(ofgen
    PRIVATE
        ${YAML_LIBRARIES}
        ${PUGIXML_LIBRARIES}
        ${JSON_LIBRARIES}
)

target_compile_options(ofgen
    PRIVATE
        ${YAML_CFLAGS_OTHER}
        ${PUGIXML_CFLAGS_OTHER}
        ${JSON_CFLAGS_OTHER}
)

# ------------------------------------------------------------------
# Optional: export a target for downstream CMake projects
# ------------------------------------------------------------------
install(TARGETS ofgen
        RUNTIME DESTINATION bin
)

# message(STATUS "YAML_INCLUDE_DIRS: ${YAML_INCLUDE_DIRS}")
# message(STATUS "YAML_LIBRARIES   : ${YAML_LIBRARIES}")
# message(STATUS "YAML_LIBRARY_DIRS: ${YAML_LIBRARY_DIRS}")

# message(STATUS "PUGIXML_INCLUDE_DIRS: ${PUGIXML_INCLUDE_DIRS}")
# message(STATUS "PUGIXML_LIBRARIES  : ${PUGIXML_LIBRARIES}")
# message(STATUS "JSON_INCLUDE_DIRS  : ${JSON_INCLUDE_DIRS}")
# message(STATUS "JSON_LIBRARIES    : ${JSON_LIBRARIES}")
