/*
 * SpotifyPlaylistContainer.hpp
 *
 *  Created on: Dec 24, 2013
 *      Author: rob
 */

#ifndef SPOTIFYPLAYLISTCONTAINER_HPP_
#define SPOTIFYPLAYLISTCONTAINER_HPP_


namespace fambogie {
class SpotifyPlaylistContainer;
class PlaylistTask;
}

#include <libspotify/api.h>

namespace fambogie {

class SpotifyPlaylistContainer {
public:
	SpotifyPlaylistContainer(sp_session* session);
	virtual ~SpotifyPlaylistContainer();

	void processTask(PlaylistTask* task);
private:
	sp_session* session;

	sp_playlistcontainer* playlistContainer;


};

} /* namespace fambogie */

#endif /* SPOTIFYPLAYLISTCONTAINER_HPP_ */