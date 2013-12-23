project(SpotterServer)

set(SOURCE
	src/Main.cpp
	src/SpotifyAlsa/audio.cpp
	src/SpotifyAlsa/alsa-audio.cpp)

set(HEADERS
	src/SpotifyAlsa/audio.h
	src/SpotifyAlsa/queue.h
	src/GlobalDefines.hpp
	src/SpotifyKeys.hpp)

add_executable(SpotterServer ${SOURCE} ${HEADERS})
target_link_libraries(SpotterServer ${LIBSPOTIFY_LIBRARIES})
target_link_libraries(SpotterServer ${ALSA_LIBRARIES})
target_link_libraries(SpotterServer ${CMAKE_THREAD_LIBS_INIT})
