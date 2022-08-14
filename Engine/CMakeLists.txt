cmake_minimum_required( VERSION "3.12" ) # Minimum For python3 find module
project( InfinitariumEngine )

if( MSVC )
else( MSCV )
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif( MSVC )

add_definitions(-DENGINE_BUILD)

set_property( GLOBAL PROPERTY USE_FOLDERS ON )

add_library( InfinitariumEngine SHARED DataBuffer.cpp MotionControllerOrbit.cpp
  PointCloud.cpp PythonBridge.cpp Renderable.cpp RenderCommand.cpp RenderPass.cpp
  RenderProgram.cpp RenderState.cpp Scene.cpp Module.cpp DataPack.hpp Module.hpp
  RenderPass.hpp Renderable.hpp EventSampler.hpp PointCloud.hpp RenderCommand.hpp
  RenderState.hpp DataBuffer.hpp MotionControllerOrbit.hpp RenderProgram.hpp Scene.hpp
  EventSampler.cpp ApplicationWindow.cpp ApplicationWindow.hpp Application.cpp
  Camera.cpp Texture.cpp Pipeline.cpp Sprite.cpp Transform.cpp SceneObject.cpp
  Spheroid.cpp CoordinateSystem.cpp Stats.cpp Stats.hpp
  Application.hpp Simulation.cpp Simulation.hpp RenderContext.cpp RenderContext.hpp
  PythonBridge.hpp EngineInterface.hpp ConsoleInterface.cpp ConsoleInterface.hpp
  PyUtil.hpp UniformType.hpp Delegate.hpp Camera.hpp Texture.hpp Pipeline.hpp
  Sprite.hpp SceneObject.hpp Transform.hpp Spheroid.hpp CoordinateSystem.hpp
  MotionController.cpp MotionController.hpp ImGUI.hpp ImGUI.cpp IImGUI.hpp IImGUI.cpp
  ObjectStore.cpp ObjectStore.hpp RenderTarget.cpp RenderTarget.hpp Database.cpp Database.hpp
  
  Terrain/Cesium.cpp Terrain/Cesium.hpp
  Terrain/GenericIOManager.cpp Terrain/GenericIOManager.hpp
  Terrain/HttpManager.cpp Terrain/HttpManager.hpp
  Terrain/LocalFileManager.cpp Terrain/LocalFileManager.hpp
  Terrain/GenericAssetAccessor.cpp Terrain/GenericAssetAccessor.hpp
  Terrain/CriticalAssetManager.cpp Terrain/CriticalAssetManager.hpp
  Terrain/HttpAssetAccessor.cpp Terrain/HttpAssetAccessor.hpp
  Terrain/TaskProcessor.cpp Terrain/TaskProcessor.hpp
  Terrain/RenderResourcesPreparer.cpp Terrain/RenderResourcesPreparer.hpp
)

source_group("Terrain" FILES 
  Terrain/Cesium.cpp Terrain/Cesium.hpp
  Terrain/GenericIOManager.cpp Terrain/GenericIOManager.hpp
  Terrain/HttpManager.cpp Terrain/HttpManager.hpp
  Terrain/LocalFileManager.cpp Terrain/LocalFileManager.hpp
  Terrain/GenericAssetAccessor.cpp Terrain/GenericAssetAccessor.hpp
  Terrain/CriticalAssetManager.cpp Terrain/CriticalAssetManager.hpp
  Terrain/HttpAssetAccessor.cpp Terrain/HttpAssetAccessor.hpp
  Terrain/TaskProcessor.cpp Terrain/TaskProcessor.hpp
  Terrain/RenderResourcesPreparer.cpp Terrain/RenderResourcesPreparer.hpp
)

set_property(TARGET InfinitariumEngine PROPERTY CXX_STANDARD 17)
set_property(TARGET InfinitariumEngine PROPERTY CXX_STANDARD_REQUIRED ON)

include_directories("../ImGui")
target_link_libraries(InfinitariumEngine ImGUI)

# boost
find_package(Boost COMPONENTS serialization python3 REQUIRED)
target_link_libraries(InfinitariumEngine ${Boost_SERIALIZATION_LIBRARY} ${Boost_PYTHON3_LIBRARY})
include_directories(${Boost_INCLUDE_DIR})

# python3
find_package(Python3 COMPONENTS Development)
target_link_libraries(InfinitariumEngine ${Python3_LIBRARIES})
include_directories(${Python3_INCLUDE_DIRS})

# SQLite
find_package(SQLite3)
include_directories(${SQLite3_INCLUDE_DIR})
target_link_libraries (InfinitariumEngine ${SQLite3_LIBRARY})

# Terrain Engine (experimental)
OPTION(CESIUM "Experimental support for terrain via Cesium Native/Cesium Ion." OFF)
if( CESIUM )
	find_path( ZLIB_INCLUDE_DIR zlib.h HINTS "C:\\Program Files (x86)\\zlib\\include" )
	include_directories(${ZLIB_INCLUDE_DIR})
	find_library( LIB_ZLIB zlib HINTS "C:\\Program Files (x86)\\zlib\\lib" )
	target_link_libraries( InfinitariumEngine ${LIB_ZLIB} )

	find_path(CESIUM_INCLUDE_DIR async++.h HINTS "C:\\Program Files (x86)\\cesium-native\\include")
	include_directories(${CESIUM_INCLUDE_DIR})
	find_path(AWS_SDK_INCLUDE_DIR aws/core/http/HttpResponse.h HINTS "C:\\Program Files (x86)\\aws-cpp-sdk-all\\include")
	include_directories(${AWS_SDK_INCLUDE_DIR})	
endif( CESIUM )

if( MSVC )
  #find_library( LIB_BCRYPT Bcrypt ) # Needed by boost::uuid
  target_link_libraries( InfinitariumEngine bcrypt )
  else( MSVC ) # On OSX and Linux we can use GMP and MPFR (multiprecision number lib) for increased big number performance.
  find_library( LIB_MPFR mpfr )
  find_library( LIB_GMP gmp )
  target_link_libraries( InfinitariumEngine ${LIB_GMP} ${LIB_MPFR} ${LIB_BCRYPT} )
endif( MSVC )

if( MSVC )
  add_compile_definitions(BOOST_ALL_NO_LIB) 
  find_path(GLM_INCLUDE_DIR glm/glm.hpp HINTS "C:\\Program Files (x86)\\glm\\include")
  include_directories(${GLM_INCLUDE_DIR})
endif( MSVC )

install(TARGETS InfinitariumEngine LIBRARY DESTINATION lib)
