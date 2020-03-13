# imgui_impl

set(BIN imgui_impl)

file(GLOB_RECURSE SRC LIST_DIRECTORIES false "*.h" "*.hpp" "*.cpp")

add_library(${BIN} ${SRC})

target_link_libraries(${BIN} libglew32d glfw3)
