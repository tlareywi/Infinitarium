cmake_minimum_required( VERSION "3.12" )
project( IEPlatform )

add_library( IEPlatform SHARED ApplicationWindow.cpp ApplicationWindow.hpp Application.cpp Application.hpp EventSampler.cpp EventSampler.hpp
)

set_property(TARGET IEPlatform PROPERTY CXX_STANDARD 17)
set_property(TARGET IEPlatform PROPERTY CXX_STANDARD_REQUIRED ON)

find_library(GLFW_LIB glfw3 HINTS "C:\\Program Files (x86)\\GLFW\\lib")
target_link_libraries(IEPlatform ${GLFW_LIB})

find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h HINTS "C:\\Program Files (x86)\\GLFW\\include")
include_directories(${GLFW_INCLUDE_DIR})

find_path(RAPIDJSON_INCLUDE_DIR rapidjson/reader.h HINTS "C:\\Program Files (x86)\\rapidjson\\include")
include_directories(${RAPIDJSON_INCLUDE_DIR})

find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h HINTS "C:\\Program Files (x86)\\VulkanSDK\\include" "C:\\local\\VulkanSDK\\include")
include_directories(${VULKAN_INCLUDE_DIR})

find_library(VULKAN_LIB NAMES vulkan-1 vulkan HINTS "C:\\Program Files (x86)\\VulkanSDK\\lib" "C:\\local\\VulkanSDK\\lib")
target_link_libraries(IEPlatform ${VULKAN_LIB})

add_compile_definitions(PLATFORM_BUILD) 

install(TARGETS IEPlatform LIBRARY DESTINATION lib)
