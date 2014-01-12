/*
 * ResponseStructures.hpp
 *
 *  Created on: Dec 28, 2013
 *      Author: rob
 */

#ifndef RESPONSESTRUCTURES_HPP_
#define RESPONSESTRUCTURES_HPP_

#include "../GlobalDefines.hpp"
#include <libspotify/api.h>

typedef struct PlaylistInfo {
	int id = -1;
	const char* name = nullptr;
	int numTracks = -1;
	const char* description;

	sp_image* image;
};

typedef struct TrackInfo {
	int id = -1;
	int duration = -1;
	int numArtists = -1;
	const char* name = nullptr;
	const char* album = nullptr;
	const char** artist = nullptr;

	sp_image* artwork;
};

#endif /* RESPONSESTRUCTURES_HPP_ */
