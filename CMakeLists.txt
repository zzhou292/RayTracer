cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
project(vulkan_vs)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-autolink")

# GLFW and Vulkan and Allegro
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/Cellar/glfw/3.3.9/lib)
include_directories(/Users/jasonzhou/VulkanSDK/1.3.275.0/macOS/include/)
link_directories(/Users/jasonzhou/VulkanSDK/1.3.275.0/macOS/lib/)

#omp
include_directories(/opt/homebrew/Cellar/libomp/17.0.6/include)
link_directories(/opt/homebrew/Cellar/libomp/17.0.6/lib)

# ImGui
include_directories(./Walnut/vendor/imgui)
file(GLOB IMGUI_SOURCES ./Walnut/vendor/imgui/*.cpp ./Walnut/vendor/imgui/backends/imgui_impl_vulkan.h ./Walnut/vendor/imgui/backends/imgui_impl_vulkan.cpp ./Walnut/vendor/imgui/backends/imgui_impl_glfw.cpp ./Walnut/vendor/imgui/backends/imgui_impl_glfw.h)
# Add backend sources if you are using them, e.g., for GLFW or OpenGL

# STB
include_directories(./Walnut/vendor/stb_image)
file(GLOB STB_SOURCES ./Walnut/vendor/stb_image/*.h)


# Walnut
include_directories(./Walnut/Walnut/src)
include_directories(./Walnut/Walnut/src/Walnut)
include_directories(./Walnut/Walnut/src/Walnut/Input)
file(GLOB WALNUT_SOURCES ./Walnut/Walnut/src/Walnut/*.cpp ./Walnut/Walnut/src/Walnut/*.h ./Walnut/Walnut/src/Walnut/Input/*.h ./Walnut/Walnut/src/Walnut/Input/*.cpp)

set(SOURCE_FILES
    WalnutApp/src/WalnutApp.cpp
    WalnutApp/src/Renderer.h
    WalnutApp/src/Renderer.cpp
    WalnutApp/src/Camera.h
    WalnutApp/src/Camera.cpp
    WalnutApp/src/Ray.h
    WalnutApp/src/Scene.h
    ${IMGUI_SOURCES}
    ${WALNUT_SOURCES}
    ${STB_SOURCES}
)

set(librariesList
    GLFW
    vulkan
    omp
)


add_executable(WalnutApp_exec ${SOURCE_FILES})
target_link_libraries(WalnutApp_exec ${librariesList})

