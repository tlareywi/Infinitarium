cmake_minimum_required( VERSION "3.12" )
project( IEPlatform )

add_library( IEPlatform SHARED ViewController.mm ApplicationWindow.mm
  ViewController.hpp ApplicationWindow.hpp Application.mm Application.hpp
)

set_property(TARGET IEPlatform PROPERTY CXX_STANDARD 17)
set_property(TARGET IEPlatform PROPERTY CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-type-c-linkage")

find_library(COCOA_LIB Cocoa)
target_link_libraries(IEPlatform ${COCOA_LIB})

find_library(WEBKIT_LIB WebKit)
target_link_libraries(IEPlatform ${WEBKIT_LIB})

install(TARGETS IEPlatform LIBRARY DESTINATION lib)
