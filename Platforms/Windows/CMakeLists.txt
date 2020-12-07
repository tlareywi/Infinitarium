cmake_minimum_required( VERSION "3.12" )
project( IEPlatform )

add_library( IEPlatform SHARED ApplicationWindow.cpp ApplicationWindow.hpp Application.cpp Application.hpp EventSampler.cpp EventSampler.hpp
)

set_property(TARGET IEPlatform PROPERTY CXX_STANDARD 17)
set_property(TARGET IEPlatform PROPERTY CXX_STANDARD_REQUIRED ON)

# boost
find_package(Boost COMPONENTS serialization REQUIRED)
target_link_libraries(IEPlatform ${Boost_SERIALIZATION_LIBRARY})

find_library(GLFW_LIB glfw3 HINTS "C:\\Program Files (x86)\\GLFW\\lib")
target_link_libraries(IEPlatform ${GLFW_LIB})

find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h HINTS "C:\\Program Files (x86)\\GLFW\\include")
include_directories(${GLFW_INCLUDE_DIR})

find_path(RAPIDJSON_INCLUDE_DIR rapidjson/reader.h HINTS "C:\\Program Files (x86)\\rapidjson\\include")
include_directories(${RAPIDJSON_INCLUDE_DIR})

# TODO: It doesn't make sense to link these in two different shared libs. Move calls that need these into rendering lib. The resulting 
# rendering lib should be make to work on both Windows and Linux, but not OSX.  
find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h HINTS "C:\\Program Files (x86)\\VulkanSDK\\include" "C:\\local\\VulkanSDK\\include")
include_directories(${VULKAN_INCLUDE_DIR})

find_library(VULKAN_LIB NAMES vulkan-1 vulkan HINTS "C:\\Program Files (x86)\\VulkanSDK\\lib" "C:\\local\\VulkanSDK\\lib")
target_link_libraries(IEPlatform ${VULKAN_LIB})

find_path(OPENXR_INCLUDE_DIR openxr.h HINTS "C:\\local\\include")
include_directories(${OPENXR_INCLUDE_DIR})

find_library(OPENXR_LIB NAMES openxr_loader HINTS "C:\\local\\lib")
target_link_libraries(IEPlatform ${OPENXR_LIB})

# Needed by openxr loader lib
target_link_libraries(IEPlatform "Pathcch.lib")

install(TARGETS IEPlatform LIBRARY DESTINATION lib)
