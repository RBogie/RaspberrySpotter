/*
 * SpotifyPlayer.cpp
 *
 *  Created on: Dec 29, 2013
 *      Author: rob
 */

#include "SpotifyPlayer.hpp"

#include <stdlib.h>
#include <string.h>

#include "SpotifySession.hpp"
#include "Tasks/PlayerTask.hpp"
#include "Responses/StatusResponse.hpp"
#include "Responses/PlayerResponse.hpp"

namespace fambogie {

SpotifyPlayer::SpotifyPlayer(sp_session* session,
		SpotifySession* spotifySession) {
	this->session = session;
	this->spotifySession = spotifySession;
	this->currentTrack = nullptr;
	this->currentTrackEnded = false;
	audio_init(&audioFifo);
}

SpotifyPlayer::~SpotifyPlayer() {
	// TODO Auto-generated destructor stub
}

void SpotifyPlayer::playTrack(sp_track* track) {
	currentTrack = track;
	audio_fifo_flush(&audioFifo);
	sp_session_player_load(session, currentTrack);
	if (sp_track_get_availability(session, track)
			== SP_TRACK_AVAILABILITY_AVAILABLE
			&& !sp_track_is_local(session, track)) {
		currentTrackEnded = false;

		sp_session_player_play(session, true);

		PlayerResponse* response = new PlayerResponse(
				PlayerResponse::PlayerResponseTypeTrackInfo);
		PlayerResponse::PlayerResponseInfo responseInfo;
		responseInfo.trackInfo = getTrackInfo(track);

		if (playQueue.size() > 0) {
			sp_session_player_prefetch(session, playQueue.front());
			responseInfo.trackInfo->nextTrack = getTrackInfo(playQueue.front());
		}
		response->setPlayerResponseInfo(responseInfo);

		logDebug("Now playing: %s", responseInfo.trackInfo->name);
		spotifySession->broadcastMessage(response);
	} else {
		currentTrackEnded = true;
	}
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
	currentTrackEnded = true;
}

void SpotifyPlayer::tick() {
	if (currentTrackEnded) {
		if (playQueue.size() > 0) {
			sp_track* track = playQueue.front();
			playQueue.pop_front();
			playTrack(track);
		} else {
			sp_session_player_unload(session);
			audio_fifo_flush(&audioFifo);
			currentTrack = nullptr;
			logDebug("No next track!");
			currentTrackEnded = false;
		}
	}
}

void SpotifyPlayer::addTrackToQueue(sp_track* track) {
	if (playQueue.size() == 0) {
		if (currentTrack == nullptr || currentTrackEnded) {
			playTrack(track);
		} else {
			sp_session_player_prefetch(session, track);
			playQueue.push_back(track);
		}
	} else {
		playQueue.push_back(track);
	}
}

void SpotifyPlayer::clearPlayQueue() {
	playQueue.clear();
}

ClientResponse* SpotifyPlayer::processTask(PlayerTask* task) {
	StatusResponse* response = new StatusResponse(false, "Unknown command");
	switch (task->getCommand()) {
	case PlayerCommandPlay:
		if (currentTrack != nullptr && !currentTrackEnded) {
			sp_session_player_play(session, true);
			response->setMessage(nullptr);
			response->setSuccess(true);
		} else {
			response->setMessage("No track to play");
		}
		break;
	case PlayerCommandPause:
		sp_session_player_play(session, false);
		response->setMessage(nullptr);
		response->setSuccess(true);
		break;
	case PlayerCommandSeek:
		if (currentTrack != nullptr && !currentTrackEnded) {
			sp_session_player_seek(session,
					task->getCommandInfo().seekPosition);
			response->setMessage(nullptr);
			response->setSuccess(true);
		} else {
			response->setMessage("No track currently playing");
		}
		break;
	}
	return response;
}

TrackInfo* SpotifyPlayer::getTrackInfo(sp_track* track) {
	if (track != nullptr) {
		TrackInfo* trackInfo = new TrackInfo();
		trackInfo->name = sp_track_name(track);
		trackInfo->numArtists = sp_track_num_artists(track);
		trackInfo->artists = new const char*[trackInfo->numArtists];
		for (int i = 0; i < trackInfo->numArtists; i++) {
			trackInfo->artists[i] = sp_artist_name(sp_track_artist(track, i));
		}
		trackInfo->duration = sp_track_duration(track);

		sp_album* album = sp_track_album(track);
		if(album != nullptr) {
			trackInfo->albumName = sp_album_name(album);
			trackInfo->albumArt = sp_image_create(session, sp_album_cover(album, SP_IMAGE_SIZE_NORMAL));
		}

		trackInfo->nextTrack = nullptr;
		return trackInfo;
	}

	return nullptr;
}

} /* namespace fambogie */
