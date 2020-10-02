cmake_minimum_required( VERSION "3.12" )
project( IERenderer )

add_library( IERenderer SHARED RenderContext.cpp RenderContext.hpp RenderCommand.cpp RenderCommand.hpp RenderState.cpp RenderState.hpp
  RenderProgram.cpp RenderProgram.hpp DataBuffer.cpp DataBuffer.hpp RenderPass.cpp RenderPass.hpp Texture.cpp Texture.hpp UniformMap.h
  default.vert.glsl default.frag.glsl starsDefault.vert.glsl starsDefault.frag.glsl
)

set_property(TARGET IERenderer PROPERTY CXX_STANDARD 17)
set_property(TARGET IERenderer PROPERTY CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-type-c-linkage")

# boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h HINTS "C:\\Program Files (x86)\\VulkanSDK\\include" "C:\\local\\VulkanSDK\\include")
include_directories(${VULKAN_INCLUDE_DIR})

find_library(VULKAN_LIB NAMES vulkan-1 vulkan HINTS "C:\\Program Files (x86)\\VulkanSDK\\lib" "C:\\local\\VulkanSDK\\lib")
target_link_libraries(IERenderer ${VULKAN_LIB})

find_library(GLSL_LIB NAMES shaderc_shared HINTS "C:\\local\\VulkanSDK\\shaderc\\build\\install\\lib" "C:\\Program Files (x86)\\VulkanSDK\\lib")
target_link_libraries(IERenderer ${GLSL_LIB})

install(TARGETS IERenderer LIBRARY DESTINATION lib)

install(FILES default.glsl.vert default.glsl.frag starsDefault.glsl.vert starsDefault.glsl.frag
  DESTINATION share/Infinitarium/shaders/vulkan)
