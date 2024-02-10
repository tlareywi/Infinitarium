cmake_minimum_required( VERSION "3.1" )
project( Infinitarium )

SET(CMAKE_INSTALL_RPATH "/usr/local/lib;.")

if( MSVC )
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

    add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
    add_compile_definitions(BOOST_ALL_NO_LIB)

    find_path(GLM_INCLUDE_DIR glm/glm.hpp HINTS "C:\\Program Files (x86)\\glm\\include")
    include_directories(${GLM_INCLUDE_DIR})
endif( MSVC )

set(CMAKE_DEBUG_POSTFIX "")

add_executable( Infinitarium main.cpp )

set_property(TARGET Infinitarium PROPERTY CXX_STANDARD 17)
set_property(TARGET Infinitarium PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("./ImGui")

# boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(Python3 COMPONENTS Development)
include_directories(${Python3_INCLUDE_DIRS})

add_subdirectory( Mechanics )

add_subdirectory( ImGui )

add_subdirectory( Renderers/Vulkan )

add_subdirectory( Engine )
target_link_libraries( Infinitarium InfinitariumEngine )

add_subdirectory( data )

configure_file( config.in ${CMAKE_SOURCE_DIR}/config.h )

if( APPLE )
    macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
        set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
    endmacro (set_xcode_property)

    # Allow resign on install (fix for SIP enabled systems)
    set(CMAKE_XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "-o linker-signed")
    set_xcode_property(Infinitarium LD_RUNPATH_SEARCH_PATHS "/usr/local/lib")
endif( APPLE )

install(TARGETS Infinitarium RUNTIME DESTINATION bin)
