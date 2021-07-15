# imgui_ext

set(SRC
        imgui_freetype.h
        imgui_freetype.cpp
        imgui_impl_glfw.h
        imgui_impl_glfw.cpp
        imgui_impl_opengl3.h
        imgui_impl_opengl3.cpp
        )

add_library(imgui_ext ${SRC})
conan_target_link_libraries(imgui_ext)
