/*
 * SpotifySession.h
 *
 *  Created on: Dec 24, 2013
 *      Author: rob
 */

#ifndef SPOTIFYSESSION_H_
#define SPOTIFYSESSION_H_

#include <map>
#include <libspotify/api.h>

#include "GlobalDefines.hpp"


namespace fambogie {
class SpotifySession;
}
#include "Tasks/Task.hpp"
#include "SpotifyRunner.hpp"
#include "SpotifyPlaylistContainer.hpp"
#include "Responses/ClientResponse.hpp"
namespace fambogie {

class SpotifySession {
public:
	/**
	 * Constructor which initializes a new SpotifySession.
	 *
	 * @param[in]	config	It needs a config which tells all kinds of information for the api.
	 * 						Since the session will take care of the callbacks, this field is ignored,
	 * 						and does not have to be set.
	 */
	SpotifySession(sp_session_config& config, SpotifyRunner* runner);

	/**
	 * This function will try to login with the given credentials. If used once before with stayLoggedIn
	 * as true, you can pass NULL as username and password, which will relogin using the old credentials.
	 *
	 * @param[in]	username		The username to login with. Allowed to be null if logged in before.
	 * @param[in]	password		The password to login with. Allowed to be null if logged in before.
	 * @param[in]	stayLoggedIn	Whether the person should stay logged in, which means that the application
	 * 								does not have to provide a username/password next time.
	 */
	void login(const char* username, const char* password, bool stayLoggedIn);

	void logout();

	/**
	 * This function will return whether an user is logged in
	 */
	bool isLoggedIn();

	/**
	 * This function will process all spotify events for the current session
	 */
	int processEvents();

	/**
	 * This function will process a given task, and will handle everything for spotify
	 */
	ClientResponse* processTask(Task* task);

	/**
	 * Destructor
	 */
	virtual ~SpotifySession();

private:
	void onLogin(sp_error error);
	void onMainThreadNotified();
	int onMusicDelivered(const sp_audioformat* format, const void* frames,
			int num_frames);
	void onEndOfTrack();

	static void onLoginDispatch(sp_session *session, sp_error error);
	static void onMainThreadNotifiedDispatch(sp_session* session);
	static int onMusicDeliveredDispatch(sp_session* session,
			const sp_audioformat* format, const void* frames, int num_frames);
	static void onLogDispatch(sp_session* session, const char* data);
	static void onEndOfTrackDispatch(sp_session* session);


	sp_session* session;
	SpotifyRunner* runner;
	sp_session_callbacks callbacks;
	bool loggedIn;
	bool errorOcurred;

	SpotifyPlaylistContainer* spotifyPlaylistContainer;

	static std::map<sp_session*, SpotifySession*> sessionMapping;
};

} /* namespace fambogie */

#endif /* SPOTIFYSESSION_H_ */
