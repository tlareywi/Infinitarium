cmake_minimum_required( VERSION "3.12" )

if( WIN32 )
    install(FILES Windows/tyco2.ieb Windows/hip2.ieb Windows/empty.ieb Windows/imgui.ieb Windows/sprite.ieb Windows/sphere.ieb 
    Windows/radiantBody.ieb infinitarium.db
    DESTINATION share/Infinitarium)
elseif( APPLE )
    install(FILES OSX/tyco2.ieb OSX/hip2.ieb OSX/empty.ieb OSX/imgui.ieb OSX/sprite.ieb OSX/sphere.ieb infinitarium.db DESTINATION share/Infinitarium)
else()
	install(FILES Linux/tyco2.ieb Linux/hip2.ieb Linux/empty.ieb Linux/imgui.ieb Linux/sprite.ieb Linux/sphere.ieb infinitarium.db DESTINATION share/Infinitarium)
endif( )