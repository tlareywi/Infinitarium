cmake_minimum_required( VERSION "3.12" ) # For python3 find module
project( InfinitariumEngine )

set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")

add_library( InfinitariumEngine SHARED DataBuffer.cpp MotionControllerOrbit.cpp
  PointCloud.cpp PythonBridge.cpp Renderable.cpp RenderCommand.cpp RenderPass.cpp
  RenderProgram.cpp RenderState.cpp Scene.cpp Module.cpp DataPack.hpp Module.hpp
  RenderPass.hpp Renderable.hpp EventSampler.hpp PointCloud.hpp RenderCommand.hpp
  RenderState.hpp DataBuffer.hpp MotionControllerOrbit.hpp RenderProgram.hpp Scene.hpp
  EventSampler.cpp ApplicationWindow.cpp ApplicationWindow.hpp Application.cpp
  Camera.cpp Texture.cpp
  Application.hpp Simulation.cpp Simulation.hpp RenderContext.cpp RenderContext.hpp
  PythonBridge.hpp EngineInterface.hpp ConsoleInterface.cpp ConsoleInterface.hpp
  PyUtil.hpp UniformType.hpp Delegate.hpp Camera.hpp Texture.hpp
)

set_property(TARGET InfinitariumEngine PROPERTY CXX_STANDARD 17)
set_property(TARGET InfinitariumEngine PROPERTY CXX_STANDARD_REQUIRED ON)

# boost
find_package(Boost COMPONENTS serialization python37 REQUIRED)
target_link_libraries(InfinitariumEngine ${Boost_SERIALIZATION_LIBRARY}
${Boost_PYTHON37_LIBRARY})
include_directories(${Boost_INCLUDE_DIR})

# python3
find_package(Python3 COMPONENTS Development)
target_link_libraries(InfinitariumEngine ${Python3_LIBRARIES})
include_directories(${Python3_INCLUDE_DIRS})

install(TARGETS InfinitariumEngine LIBRARY DESTINATION lib)
