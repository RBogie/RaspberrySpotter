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

namespace fambogie {

class SpotifyPlayer {
public:
	SpotifyPlayer(sp_session* session);
	virtual ~SpotifyPlayer();

	void playTrack(sp_track* track);
	void addTrackToQueue(sp_track* track);
	void clearPlayQueue();

	int onMusicDelivery(const sp_audioformat* format, const void* frames,
			int numFrames);
	void onEndOfTrack();

	void tick();

	ClientResponse* processTask(PlayerTask* task);

private:
	sp_session* session;
	audio_fifo_t audioFifo;

	bool currentTrackEnded;

	sp_track* currentTrack;
	std::list<sp_track*> playQueue;
};

} /* namespace fambogie */

#endif /* SPOTIFYPLAYER_HPP_ */
