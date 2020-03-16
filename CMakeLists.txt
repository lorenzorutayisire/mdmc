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

# ================================================================
# /dep
# ================================================================

# cuda
include_directories(${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})

message(${CONAN_LIBS})

target_link_libraries(${BIN} ${CONAN_LIBS})

# libnbt++
set(NBT_BUILD_TESTS OFF)
add_subdirectory("dep/libnbtplusplus")

include_directories("dep/libnbtplusplus/include")
target_link_libraries(${BIN} nbt++)

# imgui_impl
add_subdirectory("dep/imgui_impl")
include_directories("dep/imgui_impl")
target_link_libraries(${BIN} imgui_impl)

# cub
include_directories("dep/cub")

# renderdoc
include_directories("dep/renderdoc")

# tinyfiledialogs
include_directories("dep/tinyfiledialogs")
add_library(tinyfiledialogs "dep/tinyfiledialogs/tinyfiledialogs.c" "dep/tinyfiledialogs/tinyfiledialogs.h")
target_link_libraries(${BIN} tinyfiledialogs)

# ThreadPool
include_directories("dep/ThreadPool")
