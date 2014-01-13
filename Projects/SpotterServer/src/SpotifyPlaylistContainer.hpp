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

namespace fambogie {
class SpotifyPlaylistContainer;
}

#include "Responses/ClientResponse.hpp"
#include "Responses/ListResponse.hpp"
#include "Responses/StatusResponse.hpp"
#include "Responses/ResponseStructures.hpp"
#include "SpotifySession.hpp"

namespace fambogie {

class SpotifyPlaylistContainer {
public:
	SpotifyPlaylistContainer(sp_session* session, SpotifySession* spotifySession);
	virtual ~SpotifyPlaylistContainer();

	ClientResponse* processTask(PlaylistTask* task);

	ClientResponse* playPlaylist(PlaylistTask* task);
	ClientResponse* playTrack(PlaylistTask* task);
	ListResponse<PlaylistInfo*>* listPlaylists(PlaylistTask* task);
	ListResponse<TrackInfo*>* listTracks(PlaylistTask* task);
private:
	sp_session* session;
	SpotifySession* spotifySession;
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
