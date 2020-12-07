cmake_minimum_required( VERSION "3.12" )

if( MSVC )
    install(FILES Windows/tyco2.ieb Windows/hip2.ieb Windows/empty.ieb Windows/imgui.ieb DESTINATION share/Infinitarium)
else( MSVC )
    install(FILES OSX/tyco2.ieb OSX/hip2.ieb OSX/empty.ieb OSX/sprite.ieb OSX/sphere.ieb DESTINATION share/Infinitarium)
endif( MSVC )