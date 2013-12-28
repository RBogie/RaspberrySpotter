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

#include <map>
#include <libspotify/api.h>

#include "Responses/ClientResponse.hpp"
#include "Responses/ListResponse.hpp"
#include "Responses/ResponseStructures.hpp"

namespace fambogie {

class SpotifyPlaylistContainer {
public:
	SpotifyPlaylistContainer(sp_session* session);
	virtual ~SpotifyPlaylistContainer();

	ClientResponse* processTask(PlaylistTask* task);

	ListResponse<PlaylistInfo*>* listPlaylists(PlaylistTask* task);
private:
	sp_session* session;
	sp_playlistcontainer* playlistContainer;
	sp_playlistcontainer_callbacks playlistCallbacks;

	std::map<sp_playlistcontainer*, SpotifyPlaylistContainer*> containerMapper;
	static void playlistAdded(sp_playlistcontainer* pc, sp_playlist* pl,
			int position, void* userdata);
	static void playlistMoved(sp_playlistcontainer* pc, sp_playlist* playlist,
			int position, int new_position, void* userdata);
	static void playlistRemoved();

};

} /* namespace fambogie */

#endif /* SPOTIFYPLAYLISTCONTAINER_HPP_ */
