/*
 * SpotifyPlayer.hpp
 *
 *  Created on: Dec 29, 2013
 *      Author: rob
 */

#ifndef SPOTIFYPLAYER_HPP_
#define SPOTIFYPLAYER_HPP_

#include "GlobalDefines.hpp"
#include <list>
#include <libspotify/api.h>
#include "SpotifyAlsa/audio.h"

#include "Tasks/PlayerTask.hpp"
#include "Responses/ClientResponse.hpp"
#include "Responses/PlayerResponse.hpp"

namespace fambogie {

class SpotifySession;

class SpotifyPlayer {
public:
	SpotifyPlayer(sp_session* session, SpotifySession* spotifySession);
	virtual ~SpotifyPlayer();

	void playTrack(sp_track* track);
	void addTrackToQueue(sp_track* track);
	void addTrackToPlayedQueue(sp_track* track);
	void clearPlayQueue();
    void setShuffle(bool shuffle);
    bool isShuffled();

	int onMusicDelivery(const sp_audioformat* format, const void* frames,
			int numFrames);
	void onEndOfTrack();

	void tick();

	ClientResponse* processTask(PlayerTask* task);

private:
    sp_session* session;
    SpotifySession* spotifySession;
	audio_fifo_t audioFifo;

	bool currentTrackEnded;
	bool paused;

	sp_track* currentTrack;
	std::list<sp_track*> playedQueue;
	std::list<sp_track*> playQueue;
    std::list<sp_track*> currentPlayingList;
    bool shuffled;

	PlayerTrackInfo* getTrackInfo(sp_track* track);
	PlayerResponse* getCurrentPlayingResponse();
};

} /* namespace fambogie */

#endif /* SPOTIFYPLAYER_HPP_ */
