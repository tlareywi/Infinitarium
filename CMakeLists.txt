cmake_minimum_required( VERSION "3.1" )
project( Infinitarium )

add_executable( Infinitarium main.cpp
)

set_property(TARGET Infinitarium PROPERTY CXX_STANDARD 17)
set_property(TARGET Infinitarium PROPERTY CXX_STANDARD_REQUIRED ON)

# boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(Python3 COMPONENTS Development)
include_directories(${Python3_INCLUDE_DIRS})

add_subdirectory( Engine )
target_link_libraries( Infinitarium InfinitariumEngine )

if( APPLE )
    add_subdirectory( Renderers/Metal )
    add_subdirectory( Platforms/OSX )
endif( APPLE )

if( MSVC )
    add_subdirectory( Renderers/Vulkan )
    add_subdirectory( Platforms/Windows )
endif( MSVC )

add_subdirectory( data )
add_subdirectory( UI )

configure_file( config.in ${CMAKE_SOURCE_DIR}/config.h )
