# imgui_ext

set(BIN imgui_ext)

file(GLOB_RECURSE SRC LIST_DIRECTORIES false "*.h" "*.hpp" "*.cpp")

include_directories("include")

add_library(${BIN} ${SRC})

target_link_libraries(${BIN} libglew32d glfw3)
