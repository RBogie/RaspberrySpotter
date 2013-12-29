/*
 * SpotifyPlayer.cpp
 *
 *  Created on: Dec 29, 2013
 *      Author: rob
 */

#include "SpotifyPlayer.hpp"

#include <stdlib.h>
#include <string.h>

namespace fambogie {

SpotifyPlayer::SpotifyPlayer(sp_session* session) {
	this->session = session;
	this->currentTrack = nullptr;
	audio_init(&audioFifo);
}

SpotifyPlayer::~SpotifyPlayer() {
	// TODO Auto-generated destructor stub
}

void SpotifyPlayer::playTrack(sp_track* track) {
	currentTrack = track;
	sp_session_player_load(session, currentTrack);
	//sp_session_player_seek(session, 30000);
	sp_session_player_play(session, true);
}

int SpotifyPlayer::onMusicDelivery(const sp_audioformat* format,
		const void* frames, int numFrames) {
	if (numFrames == 0) { //Audio discontinued
		return 0;
	}

	audio_fifo_t* af = &audioFifo;

	pthread_mutex_lock(&af->mutex);

	if (af->qlen > format->sample_rate) {
		pthread_mutex_unlock(&af->mutex);
		return 0;
	}

	size_t s = numFrames * sizeof(int16_t) * format->channels;

	audio_fifo_data_t* afd = (audio_fifo_data_t*) malloc(sizeof(*afd) + s);
	memcpy(afd->samples, frames, s);

	afd->nsamples = numFrames;
	afd->rate = format->sample_rate;
	afd->channels = format->channels;

	TAILQ_INSERT_TAIL(&af->q, afd, link);
	af->qlen += numFrames;

	pthread_cond_signal(&af->cond);
	pthread_mutex_unlock(&af->mutex);

	return numFrames;
}

void SpotifyPlayer::onEndOfTrack() {
	logDebug("Track ended");
	currentTrack == nullptr;
	sp_session_player_unload(session);
	audio_fifo_flush(&audioFifo);
}

} /* namespace fambogie */
