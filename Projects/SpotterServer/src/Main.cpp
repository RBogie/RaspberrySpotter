#include <cstring>

#include "GlobalDefines.hpp"
#include "SpotifyKeys.hpp"
#include "SpotifyRunner.hpp"
#include "TaskInformation.hpp"
#include <unistd.h>

int main(int argc, const char* argv[]) {
	sp_session_config config;
	memset(&config, 0, sizeof(config));

	config.api_version = SPOTIFY_API_VERSION;
	config.cache_location = "tmp";
	config.settings_location = "tmp";
	config.application_key = SpotifyKeys::appkey;
	config.application_key_size = SpotifyKeys::appkeySize;
	config.user_agent = "RaspberrySpotter";

	fambogie::SpotifyRunner runner(config);
	runner.run();

	sleep(5);
	fambogie::TaskInformation* task = new fambogie::TaskInformation();
	task->command = fambogie::ListPlaylists;
	runner.addTask(task);

	sleep(3600);
}
