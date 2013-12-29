/*
 * SpotifySession.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: rob
 */

#include "SpotifySession.hpp"

#include <unistd.h>

#include "Tasks/PlaylistTask.h"

namespace fambogie {

std::map<sp_session*, SpotifySession*> SpotifySession::sessionMapping;

SpotifySession::SpotifySession(sp_session_config& config,
		SpotifyRunner* runner) {
	this->runner = runner;

	spotifyPlayer = nullptr;
	spotifyPlaylistContainer = nullptr;

	callbacks.logged_in = &onLoginDispatch;
	callbacks.notify_main_thread = &onMainThreadNotifiedDispatch;
	callbacks.music_delivery = &onMusicDeliveryDispatch;
	callbacks.log_message = &onLogDispatch;
	callbacks.end_of_track = &onEndOfTrackDispatch;

	config.callbacks = &this->callbacks;

	sp_error err = sp_session_create(&config, &session);
	if (err != SP_ERROR_OK) {
		logError("Unable to create session: %s", sp_error_message(err));
	}

	loggedIn = false;
	errorOcurred = false;

	//Add the session to the mapping, used by the dispatchers.
	sessionMapping[session] = this;
}

SpotifySession::~SpotifySession() {
	if(loggedIn) {
		this->logout();
	}

	//Remove the session from the mapping
	sessionMapping.erase(session);

	sp_session_release(session);
	session = nullptr;
}

int SpotifySession::processEvents() {
	int nextTimeOut;
	sp_session_process_events(session, &nextTimeOut);

	if(spotifyPlayer != nullptr) {
		spotifyPlayer->tick();
	}

	return nextTimeOut;
}

void SpotifySession::login(const char* username, const char* password,
		bool stayLoggedIn) {
	sp_error err;

	if (username == nullptr || password == nullptr) {
		err = sp_session_relogin(session);
		if (err != SP_ERROR_OK) {
			logError("Could not login: No credentials found");
		} else {
			loggedIn = true;
		}
	} else {
		err = sp_session_login(session, username, password, stayLoggedIn,
				nullptr);
		if (err != SP_ERROR_OK) {
			logError(
					"Something unexpected happen (Function should always succeed).");
		} else {
			while (!loggedIn && !errorOcurred) {
				int timeout = processEvents();
				logDebug("Timeout: %d", timeout);
				usleep(timeout);
			}

			//Let them retry, by calling this function again. Simply the not logged in status is enough for us.
			if (errorOcurred) {
				logError("Error occured");
				errorOcurred = false;
			} else {
				loggedIn = true;
			}
		}
	}

	if(loggedIn) {
		logDebug("Logged in");
		spotifyPlayer = new SpotifyPlayer(session);
		spotifyPlaylistContainer = new SpotifyPlaylistContainer(session, this);
	}
}

void SpotifySession::logout() {
	if(loggedIn) {
		delete spotifyPlaylistContainer;
		delete spotifyPlayer;
		spotifyPlaylistContainer = nullptr;
		spotifyPlayer = nullptr;
	}
	sp_session_logout(session);
}

ClientResponse* SpotifySession::processTask(Task* task) {
	ClientResponse* response = nullptr;
	switch(task->getType()) {
	case TaskTypePlaylist:
		response = spotifyPlaylistContainer->processTask(dynamic_cast<PlaylistTask*>(task));
		break;
	default:
		logError("Received unknown command");
	}
	delete task;
	return response;
}

void SpotifySession::onLogin(sp_error error) {
	logDebug("Onlogin called");
	if (error != SP_ERROR_OK) {
		errorOcurred = true;
		logError("Error occured on login: %s", sp_error_message(error));
	} else {
		loggedIn = true;
	}
}

void SpotifySession::onMainThreadNotified() {
	logDebug("Mainthread notified");
	if (loggedIn) {
		runner->notify();
	}
}

int SpotifySession::onMusicDelivery(const sp_audioformat* format,
		const void* frames, int num_frames) {
	if(spotifyPlayer != nullptr) {
		return spotifyPlayer->onMusicDelivery(format, frames, num_frames);
	}
	return 0;
}

void SpotifySession::onEndOfTrack() {
	logDebug("Track ended");
	if(spotifyPlayer != nullptr) {
		return spotifyPlayer->onEndOfTrack();
	}
}

SpotifyPlayer* SpotifySession::getSpotifyPlayer() {
	return spotifyPlayer;
}

//Static dispatchers
void SpotifySession::onLoginDispatch(sp_session* session, sp_error error) {

	std::map<sp_session*, SpotifySession*>::iterator iterator =
			sessionMapping.find(session);

	if (iterator != sessionMapping.end()) {
		iterator->second->onLogin(error);
	} else {
		logError("Could not find session: %p", (void*) session);
	}
}

void SpotifySession::onMainThreadNotifiedDispatch(sp_session* session) {

	std::map<sp_session*, SpotifySession*>::iterator iterator =
			sessionMapping.find(session);

	if (iterator != sessionMapping.end()) {
		iterator->second->onMainThreadNotified();
	}
}

int SpotifySession::onMusicDeliveryDispatch(sp_session* session,
		const sp_audioformat* format, const void* frames, int num_frames) {

	std::map<sp_session*, SpotifySession*>::iterator iterator =
			sessionMapping.find(session);

	if (iterator != sessionMapping.end()) {
		iterator->second->onMusicDelivery(format, frames, num_frames);
	} else {
		logError("Could not find session: %p", (void*) session);
	}
}

void SpotifySession::onLogDispatch(sp_session* session, const char* data) {
	logDebug("Spotify logging: %s", data);
}

void SpotifySession::onEndOfTrackDispatch(sp_session* session) {

	std::map<sp_session*, SpotifySession*>::iterator iterator =
			sessionMapping.find(session);

	if (iterator != sessionMapping.end()) {
		iterator->second->onEndOfTrack();
	} else {
		logError("Could not find session: %p", (void*) session);
	}
}

} /* namespace fambogie */
