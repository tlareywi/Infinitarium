cmake_minimum_required( VERSION "3.1" )
project( Infinitarium )

add_executable( Infinitarium main.cpp
)

set_property(TARGET Infinitarium PROPERTY CXX_STANDARD 17)
set_property(TARGET Infinitarium PROPERTY CXX_STANDARD_REQUIRED ON)

# boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory( Engine )
target_link_libraries( Infinitarium InfinitariumEngine )

add_subdirectory( Renderers/Metal )
add_subdirectory( Platforms/OSX )
