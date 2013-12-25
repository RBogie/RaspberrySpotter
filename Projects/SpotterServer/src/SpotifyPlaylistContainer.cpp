/*
 * SpotifyPlaylistContainer.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: rob
 */

#include "SpotifyPlaylistContainer.hpp"

namespace fambogie {

static void playlist_added(sp_playlistcontainer *pc, sp_playlist *pl,
                           int position, void *userdata)
{

}

static void playlist_removed(sp_playlistcontainer *pc, sp_playlist *pl,
                             int position, void *userdata)
{
}


static void container_loaded(sp_playlistcontainer *pc, void *userdata)
{
    logDebug("jukebox: Rootlist synchronized (%d playlists)\n",
        sp_playlistcontainer_num_playlists(pc));
}

SpotifyPlaylistContainer::SpotifyPlaylistContainer(sp_session* session) {
	this->session = session;
	playlistContainer = sp_session_playlistcontainer(session);

	sp_playlistcontainer_callbacks playlistCallbacks;
	playlistCallbacks.container_loaded = &container_loaded;
	playlistCallbacks.playlist_added = &playlist_added;
	playlistCallbacks.playlist_removed = &playlist_removed;

	sp_playlistcontainer_add_callbacks(
	        playlistContainer,
	        &playlistCallbacks,
	        NULL);
}

void SpotifyPlaylistContainer::processTask(TaskInformation* task) {
	switch(task->command) {
	case ListPlaylists:
		int numPlaylists = sp_playlistcontainer_num_playlists(playlistContainer);
		for (int i = 0; i < numPlaylists; i++) {
			sp_playlist* playlist = sp_playlistcontainer_playlist(playlistContainer, i);
			logDebug(sp_playlist_name(playlist));
		}
	}
}

SpotifyPlaylistContainer::~SpotifyPlaylistContainer() {

}

} /* namespace fambogie */
