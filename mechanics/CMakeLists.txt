cmake_minimum_required( VERSION "3.1" )
project( IEMechanics )

add_library( IEMechanics SHARED 
    OrbitalFactory.cpp OrbitalFactory.hpp Calendar.cpp Calendar.hpp
)

set_property(TARGET IEMechanics PROPERTY CXX_STANDARD 17)
set_property(TARGET IEMechanics PROPERTY CXX_STANDARD_REQUIRED ON)

add_subdirectory( libnova )

include_directories("./libnova/src")
target_link_libraries(IEMechanics libnova)

install(TARGETS IEMechanics LIBRARY DESTINATION lib)
