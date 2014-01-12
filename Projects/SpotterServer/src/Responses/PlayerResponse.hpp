/*
 * PlayerResponse.hpp
 *
 *  Created on: Jan 10, 2014
 *      Author: rob
 */

#ifndef PLAYERRESPONSE_HPP_
#define PLAYERRESPONSE_HPP_

#include "../GlobalDefines.hpp"
#include <libspotify/api.h>

namespace fambogie {

struct PlayerTrackInfo {
	const char* name;
	const char** artists;
	int numArtists;
	int duration;
	const char* albumName;
	sp_image* albumArt;
	PlayerTrackInfo* nextTrack;
};

class PlayerResponse: public ClientResponse {
public:

	union PlayerResponseInfo {
		PlayerTrackInfo* trackInfo;
	};

	enum PlayerResponseType {
		PlayerResponseTypeUnknown, PlayerResponseTypeTrackInfo,
	};

	PlayerResponse(PlayerResponseType type) :
			ClientResponse(Player) {
		this->playerResponseType = type;
		this->currentlyPlaying = false;
		switch (type) {
		case PlayerResponseTypeTrackInfo:
			info.trackInfo = nullptr;
			break;
		}
	}

	~PlayerResponse() {
		switch (playerResponseType) {
		case PlayerResponseTypeTrackInfo:
			if (info.trackInfo != nullptr) {
				PlayerTrackInfo* trackToDelete = info.trackInfo;
				while (trackToDelete != nullptr) {
					PlayerTrackInfo* currentTrack = trackToDelete;
					trackToDelete = currentTrack->nextTrack;
					if (currentTrack->albumArt != nullptr) {
						sp_image_release(currentTrack->albumArt);
					}
					delete[] currentTrack->artists;
					delete currentTrack;
				}
			}
			break;
		}
	}

	PlayerResponseType getPlayerResponseType() {
		return playerResponseType;
	}

	PlayerResponseInfo getPlayerResponseInfo() {
		return info;
	}
	void setPlayerResponseInfo(PlayerResponseInfo info) {
		this->info = info;
	}

	bool isCurrentlyPlaying() {
		return currentlyPlaying;
	}
	void setCurrentlyPlaying(bool currentlyPlaying) {
		this->currentlyPlaying = currentlyPlaying;
	}

private:
	PlayerResponseType playerResponseType;
	PlayerResponseInfo info;
	bool currentlyPlaying;
};

} /* namespace fambogie */
#endif /* PLAYERRESPONSE_HPP_ */
