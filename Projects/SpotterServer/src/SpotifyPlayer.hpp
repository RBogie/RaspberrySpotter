/*
 * SpotifyPlayer.hpp
 *
 *  Created on: Dec 29, 2013
 *      Author: rob
 */

#ifndef SPOTIFYPLAYER_HPP_
#define SPOTIFYPLAYER_HPP_

#include "GlobalDefines.hpp"

#include <libspotify/api.h>
#include "SpotifyAlsa/audio.h"
namespace fambogie {

class SpotifyPlayer {
public:
	SpotifyPlayer(sp_session* session);
	virtual ~SpotifyPlayer();

	void playTrack(sp_track* track);

	int onMusicDelivery(const sp_audioformat* format, const void* frames,
			int numFrames);
	void onEndOfTrack();
private:
	sp_session* session;
	audio_fifo_t audioFifo;

	sp_track* currentTrack;
};

} /* namespace fambogie */

#endif /* SPOTIFYPLAYER_HPP_ */
