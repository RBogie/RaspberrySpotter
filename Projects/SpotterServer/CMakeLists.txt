project(SpotterServer)

set(SOURCE
	src/Main.cpp
	src/Logging.cpp
	src/SpotifyPlaylistContainer.cpp
	src/SpotifyRunner.cpp
	src/SpotifySession.cpp
	src/ThreadingUtils.cpp
	src/SpotifyAlsa/audio.cpp
	src/SpotifyAlsa/alsa-audio.cpp)

set(HEADERS
	src/SpotifyPlaylistContainer.hpp
	src/SpotifyRunner.hpp
	src/SpotifySession.hpp
	src/ThreadingUtils.hpp
	src/GlobalDefines.hpp
	src/SpotifyKeys.hpp
	src/SpotifyAlsa/audio.h
	src/SpotifyAlsa/queue.h)

add_executable(SpotterServer ${SOURCE} ${HEADERS})
target_link_libraries(SpotterServer ${LIBSPOTIFY_LIBRARIES})
target_link_libraries(SpotterServer ${ALSA_LIBRARIES})
target_link_libraries(SpotterServer ${CMAKE_THREAD_LIBS_INIT})
