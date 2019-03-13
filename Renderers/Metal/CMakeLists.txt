cmake_minimum_required( VERSION "3.12" )
project( IERenderer )

add_library( IERenderer SHARED RenderContext.mm RenderContext.h RenderCommand.mm RenderCommand.h RenderState.mm RenderState.h
  RenderProgram.mm RenderProgram.h DataBuffer.mm DataBuffer.h RenderPass.mm RenderPass.h UniformMap.h
  starsDefault.metal default.metal stars_no_plx.metal
)

set_property(TARGET IERenderer PROPERTY CXX_STANDARD 17)
set_property(TARGET IERenderer PROPERTY CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-type-c-linkage")

# Enable ARC
# target_compile_options(IERenderer PUBLIC "-fobjc-arc")

# boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_library(METAL_LIB Metal)
target_link_libraries(IERenderer ${METAL_LIB})

find_library(QUARTZ_LIB QuartzCore)
target_link_libraries(IERenderer ${QUARTZ_LIB})

install(TARGETS IERenderer LIBRARY DESTINATION lib)

install(FILES starsDefault.metal stars_no_plx.metal default.metal DESTINATION share/Infinitarium/shaders/metal)
