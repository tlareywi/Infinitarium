cmake_minimum_required( VERSION "3.12" )
project( ImGUI )

if( APPLE )
add_library( ImGUI SHARED 
  imgui.cpp imgui_widgets.cpp imgui_draw.cpp imgui_demo.cpp
  examples/imgui_impl_metal.mm
  examples/imgui_impl_osx.mm
)
else( APPLE )
add_library( ImGUI SHARED 
  imgui.cpp imgui_widgets.cpp imgui_draw.cpp imgui_demo.cpp
  examples/imgui_impl_vulkan.cpp 
  examples/imgui_impl_glfw.cpp
)

find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h HINTS "C:\\Program Files (x86)\\VulkanSDK\\include" "C:\\local\\VulkanSDK\\include")
include_directories(${VULKAN_INCLUDE_DIR})
find_library(VULKAN_LIB NAMES vulkan-1 vulkan HINTS "C:\\Program Files (x86)\\VulkanSDK\\lib" "C:\\local\\VulkanSDK\\lib")
target_link_libraries(ImGUI ${VULKAN_LIB})

find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h HINTS "C:\\Program Files (x86)\\GLFW\\include")
include_directories(${GLFW_INCLUDE_DIR})
find_library(GLFW_LIB NAMES glfw3 glfw HINTS "C:\\Program Files (x86)\\GLFW\\lib")
target_link_libraries(ImGUI ${GLFW_LIB})
endif( APPLE )  

include_directories(".")

if( MSVC )
  set_target_properties(ImGUI PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
else( MSCV )
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif( MSVC )

install(TARGETS ImGUI LIBRARY DESTINATION lib)