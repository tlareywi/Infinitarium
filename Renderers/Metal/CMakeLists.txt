cmake_minimum_required( VERSION "3.12" )
project( IERenderer )

add_library( IERenderer SHARED EngineImpl.mm EngineImpl.hpp ShaderTypes.h
  Shaders.metal
)

set_property(TARGET IERenderer PROPERTY CXX_STANDARD 17)
set_property(TARGET IERenderer PROPERTY CXX_STANDARD_REQUIRED ON)

# boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_library(METAL_LIB Metal)
target_link_libraries(IERenderer ${METAL_LIB})

find_library(QUARTZ_LIB QuartzCore)
target_link_libraries(IERenderer ${QUARTZ_LIB})

install(TARGETS IERenderer LIBRARY DESTINATION lib)
