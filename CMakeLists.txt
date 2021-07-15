cmake_minimum_required (VERSION 3.8)

project("mdmc" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# conan
include("conanbuildinfo.cmake")
conan_basic_setup()

# ================================================================
# /resources
# ================================================================

file(GLOB RESOURCES "resources/*")
file(COPY ${RESOURCES} DESTINATION "${CMAKE_BINARY_DIR}/bin/resources")

# ================================================================
# /src
# ================================================================

set(BIN ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SRC LIST_DIRECTORIES false "src/*.h" "src/*.hpp" "src/*.cpp" "src/*.cuh" "src/*.cu")

include_directories("src")

add_executable(${BIN} ${SRC})
conan_target_link_libraries(${BIN})

# ================================================================
# /dep
# ================================================================

# libnbt++
set(NBT_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory("dep/libnbtplusplus")

include_directories("dep/libnbtplusplus/include")
target_link_libraries(${BIN} nbt++)

#imgui_ext
add_subdirectory("dep/imgui")
include_directories("dep/imgui")
target_link_libraries(${BIN} imgui_ext)

# renderdoc
include_directories("dep/renderdoc")

# tinyfiledialogs
include_directories("dep/tinyfiledialogs")
add_library(tinyfiledialogs "dep/tinyfiledialogs/tinyfiledialogs.c" "dep/tinyfiledialogs/tinyfiledialogs.h")
target_link_libraries(${BIN} tinyfiledialogs)

# ThreadPool
include_directories("dep/ThreadPool")

# hat-trie
include_directories("dep/hat-trie/include")

# libzip
add_subdirectory("dep/libzip")
include_directories("dep/libzip/lib")
target_link_libraries(${BIN} zip)
