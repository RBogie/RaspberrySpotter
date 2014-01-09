project(SpotterServer)

set(SOURCE
	src/Main.cpp
	src/MessageConversion.cpp
	src/Logging.cpp
	src/SpotifyPlayer.cpp
	src/SpotifyPlaylistContainer.cpp
	src/SpotifyRunner.cpp
	src/SpotifySession.cpp
	src/TcpConnection.cpp
	src/TcpServer.cpp
	src/Thread.cpp
	src/ThreadingUtils.cpp
	src/SpotifyAlsa/audio.cpp
	src/SpotifyAlsa/alsa-audio.cpp
	src/Responses/ClientResponse.cpp
	src/Tasks/PlayerTask.cpp
	src/Tasks/PlaylistTask.cpp
	src/Tasks/Task.cpp
	src/PropertyParser/propertyutil.cpp)

add_executable(SpotterServer ${SOURCE})
target_link_libraries(SpotterServer ${LIBSPOTIFY_LIBRARIES})
target_link_libraries(SpotterServer ${ALSA_LIBRARIES})
target_link_libraries(SpotterServer ${CMAKE_THREAD_LIBS_INIT})
