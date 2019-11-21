cmake_minimum_required( VERSION "3.12" ) # For python3 find module
project( InfinitariumEngine )

if( MSVC )
else( MSCV )
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif( MSVC )

add_definitions(-DENGINE_BUILD)

add_library( InfinitariumEngine SHARED DataBuffer.cpp MotionControllerOrbit.cpp
  PointCloud.cpp PythonBridge.cpp Renderable.cpp RenderCommand.cpp RenderPass.cpp
  RenderProgram.cpp RenderState.cpp Scene.cpp Module.cpp DataPack.hpp Module.hpp
  RenderPass.hpp Renderable.hpp EventSampler.hpp PointCloud.hpp RenderCommand.hpp
  RenderState.hpp DataBuffer.hpp MotionControllerOrbit.hpp RenderProgram.hpp Scene.hpp
  EventSampler.cpp ApplicationWindow.cpp ApplicationWindow.hpp Application.cpp
  Camera.cpp Texture.cpp Pipeline.cpp Sprite.cpp Transform.cpp SceneObject.cpp
  Spheroid.cpp CoordinateSystem.cpp
  Application.hpp Simulation.cpp Simulation.hpp RenderContext.cpp RenderContext.hpp
  PythonBridge.hpp EngineInterface.hpp ConsoleInterface.cpp ConsoleInterface.hpp
  PyUtil.hpp UniformType.hpp Delegate.hpp Camera.hpp Texture.hpp Pipeline.hpp
  Sprite.hpp SceneObject.hpp Transform.hpp Spheroid.hpp CoordinateSystem.hpp
  MotionController.cpp MotionController.hpp
)

set_property(TARGET InfinitariumEngine PROPERTY CXX_STANDARD 17)
set_property(TARGET InfinitariumEngine PROPERTY CXX_STANDARD_REQUIRED ON)

# boost
find_package(Boost COMPONENTS serialization wserialization python37 REQUIRED)
target_link_libraries(InfinitariumEngine ${Boost_WSERIALIZATION_LIBRARY} ${Boost_SERIALIZATION_LIBRARY} ${Boost_PYTHON37_LIBRARY})
include_directories(${Boost_INCLUDE_DIR})

# python3
find_package(Python3 COMPONENTS Development)
target_link_libraries(InfinitariumEngine ${Python3_LIBRARIES})
include_directories(${Python3_INCLUDE_DIRS})

if( MSVC )
else( MSVC ) # On OSX and Linux we can use GMP and MPFR (multiprecision number lib) for increased big number performance.
find_library( LIB_MPFR mpfr )
find_library( LIB_GMP gmp )
target_link_libraries( InfinitariumEngine ${LIB_GMP} ${LIB_MPFR} )
endif( MSVC )

if( MSVC )
  add_compile_definitions(BOOST_ALL_NO_LIB) 
  find_path(GLM_INCLUDE_DIR glm/glm.hpp HINTS "C:\\Program Files (x86)\\glm\\include")
  include_directories(${GLM_INCLUDE_DIR})
  find_path(RAPIDJSON_INCLUDE_DIR rapidjson/reader.h HINTS "C:\\Program Files (x86)\\rapidjson\\include")
  include_directories(${RAPIDJSON_INCLUDE_DIR})
endif( MSVC )

install(TARGETS InfinitariumEngine LIBRARY DESTINATION lib)
