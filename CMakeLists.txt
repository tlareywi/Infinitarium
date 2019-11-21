cmake_minimum_required( VERSION "3.1" )
project( Infinitarium )

if( MSVC )
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

    add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
    add_compile_definitions(BOOST_ALL_NO_LIB)

    find_path(GLM_INCLUDE_DIR glm/glm.hpp HINTS "C:\\Program Files (x86)\\glm\\include")
    include_directories(${GLM_INCLUDE_DIR})
    find_path(RAPIDJSON_INCLUDE_DIR rapidjson/reader.h HINTS "C:\\Program Files (x86)\\rapidjson\\include")
    include_directories(${RAPIDJSON_INCLUDE_DIR})
endif( MSVC )

add_executable( Infinitarium main.cpp
)

set_property(TARGET Infinitarium PROPERTY CXX_STANDARD 17)
set_property(TARGET Infinitarium PROPERTY CXX_STANDARD_REQUIRED ON)

# boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(Python3 COMPONENTS Development)
include_directories(${Python3_INCLUDE_DIRS})

if( MSVC )
  find_path(GMP_INCLUDE_DIR gmp.h HINTS "C:\\local\\include")
  include_directories(${GMP_INCLUDE_DIR})
endif( MSVC )

if( APPLE )
    add_subdirectory( Renderers/Metal )
    add_subdirectory( Platforms/OSX )
endif( APPLE )
if( MSVC )
    add_subdirectory( Renderers/Vulkan )
    add_subdirectory( Platforms/Windows )
endif( MSVC )

add_subdirectory( Engine )
target_link_libraries( Infinitarium InfinitariumEngine )

add_subdirectory( data )
add_subdirectory( UI )

configure_file( config.in ${CMAKE_SOURCE_DIR}/config.h )
