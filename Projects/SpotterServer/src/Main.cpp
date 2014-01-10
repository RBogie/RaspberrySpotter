#include <cstring>

#include "GlobalDefines.hpp"
#include "SpotifyKeys.hpp"
#include "SpotifyRunner.hpp"
#include "TcpServer.hpp"
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
	fambogie::TcpServer server(6666, 8, &runner);
	runner.setTcpServer(&server);
	runner.start();
	server.listen();
}
