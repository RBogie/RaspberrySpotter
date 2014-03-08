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

struct PlaylistInfo {
    int id;
    const char* name;
    int numTracks;
	const char* description;

	sp_image* image;
};

struct TrackInfo {
    int id;
    int duration;
    int numArtists;
    const char* name;
    const char* album;
    const char** artists;

	sp_image* artwork;
};

#endif /* RESPONSESTRUCTURES_HPP_ */
