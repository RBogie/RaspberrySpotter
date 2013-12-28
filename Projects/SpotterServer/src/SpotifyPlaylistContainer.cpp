/*
 * SpotifyPlaylistContainer.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: rob
 */

#include "SpotifyPlaylistContainer.hpp"
#include "Tasks/PlaylistTask.h"

namespace fambogie {

static void playlist_added(sp_playlistcontainer *pc, sp_playlist *pl,
		int position, void *userdata) {

}

static void playlist_removed(sp_playlistcontainer *pc, sp_playlist *pl,
		int position, void *userdata) {
}

static void playlist_moved(sp_playlistcontainer *pc, sp_playlist *playlist,
		int position, int new_position, void *userdata) {
}

static void container_loaded(sp_playlistcontainer *pc, void *userdata) {
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
	playlistCallbacks.playlist_moved = &playlist_moved;

	sp_playlistcontainer_add_callbacks(playlistContainer, &playlistCallbacks,
	nullptr);
}

ClientResponse* SpotifyPlaylistContainer::processTask(PlaylistTask* task) {
	ClientResponse* response = nullptr;
	switch (task->getCommand()) {
	case CommandList:
		return listPlaylists(task);
		break;
	}
	return response;
}

ListResponse<PlaylistInfo*>* SpotifyPlaylistContainer::listPlaylists(
		PlaylistTask* task) {
	ListResponse<PlaylistInfo*>* response = new ListResponse<PlaylistInfo*>();
	response->setListType(ListTypePlaylist);
	CommandInfo info = task->getCommandInfo();
	bool sendName			= (info.ListFlags & Name == Name);
	bool sendNumTracks		= (info.ListFlags & NumTracks == NumTracks);
	bool sendDescription	= (info.ListFlags & Description == Description);

	int numPlaylists = sp_playlistcontainer_num_playlists(playlistContainer);
	for (int i = 0; i < numPlaylists; i++) {
		PlaylistInfo* playlistInfo = new PlaylistInfo();
		playlistInfo->id = i;
		sp_playlist* playlist = sp_playlistcontainer_playlist(playlistContainer, i);
		if(sendName) {
			playlistInfo->name = sp_playlist_name(playlist);
		}
		if(sendNumTracks) {
			playlistInfo->numTracks = sp_playlist_num_tracks(playlist);
		}
		if(sendDescription) {
			playlistInfo->description = sp_playlist_get_description(playlist);
		}
		response->addMember(playlistInfo);
	}
	return response;
}

SpotifyPlaylistContainer::~SpotifyPlaylistContainer() {
	sp_playlistcontainer_remove_callbacks(playlistContainer, &playlistCallbacks,
	nullptr);
}

} /* namespace fambogie */
