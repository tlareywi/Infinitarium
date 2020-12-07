cmake_minimum_required( VERSION "3.12" )
project( IERenderer )

add_library( IERenderer SHARED RenderContext.cpp RenderContext.hpp RenderCommand.cpp RenderCommand.hpp RenderState.cpp RenderState.hpp
  RenderProgram.cpp RenderProgram.hpp DataBuffer.cpp DataBuffer.hpp RenderPass.cpp RenderPass.hpp Texture.cpp Texture.hpp RenderTarget.cpp
  RenderTarget.hpp 
  # TODO ImGUI symbols are duplicated in Engine dll. Need to factor this into a separate dll.
  ../../Engine/ImGUI/examples/imgui_impl_vulkan.cpp ../../Engine/ImGUI/examples/imgui_impl_vulkan.h  ../../Engine/ImGUI/imgui.cpp ../../Engine/ImGUI/imgui_widgets.cpp ../../Engine/ImGUI/imgui_draw.cpp
  OpenXRContext.cpp OpenXRContext.hpp xr_linear.h ImGUI.cpp ImGUI.hpp
  UniformMap.h default.vert.glsl default.frag.glsl starsDefault.vert.glsl starsDefault.frag.glsl starsUnitSphere.vert.glsl starsUnitSphere.frag.glsl
)

set_property(TARGET IERenderer PROPERTY CXX_STANDARD 17)
set_property(TARGET IERenderer PROPERTY CXX_STANDARD_REQUIRED ON)

include_directories("../../Engine/ImGUI")

# boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
find_package(Boost COMPONENTS serialization REQUIRED)
target_link_libraries(IERenderer ${Boost_SERIALIZATION_LIBRARY})

find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h HINTS "C:\\Program Files (x86)\\VulkanSDK\\include" "C:\\local\\VulkanSDK\\include")
include_directories(${VULKAN_INCLUDE_DIR})

find_library(VULKAN_LIB NAMES vulkan-1 vulkan HINTS "C:\\Program Files (x86)\\VulkanSDK\\lib" "C:\\local\\VulkanSDK\\lib")
target_link_libraries(IERenderer ${VULKAN_LIB})

find_library(GLSL_LIB NAMES shaderc_shared HINTS "C:\\local\\VulkanSDK\\shaderc\\build\\install\\lib" "C:\\Program Files (x86)\\VulkanSDK\\lib")
target_link_libraries(IERenderer ${GLSL_LIB})

find_path(OPENXR_INCLUDE_DIR openxr.h HINTS "C:\\local\\include")
include_directories(${OPENXR_INCLUDE_DIR})

find_library(OPENXR_LIB NAMES openxr_loader HINTS "C:\\local\\lib")
target_link_libraries(IERenderer ${OPENXR_LIB})

# Needed by openxr loader lib
target_link_libraries(IERenderer "Pathcch.lib")

install(TARGETS IERenderer LIBRARY DESTINATION lib)

install(FILES default.vert.glsl default.frag.glsl starsDefault.vert.glsl starsDefault.frag.glsl starsUnitSphere.vert.glsl starsUnitSphere.frag.glsl
  DESTINATION share/Infinitarium/shaders/glsl)
