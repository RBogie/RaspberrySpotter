/*
 * PlaylistTask.h
 *
 *  Created on: Dec 27, 2013
 *      Author: rob
 */

#ifndef PLAYLISTTASK_H_
#define PLAYLISTTASK_H_

namespace fambogie {
class PlaylistTask;
}

#include "Task.hpp"

namespace fambogie {

enum Command {
	CommandUnknown,
	CommandList,
	CommandListTracks,
	CommandPlayPlaylist,
	CommandPlayTrack,
};

enum ListFlags {
	Name = 1,
	NumTracks = 2,
	Description = 4,
	Image = 8,
};

enum TrackInfoFlags {
	TrackInfoName = 1,
	TrackInfoDuration = 2,
	TrackInfoAlbum = 4,
	TrackInfoArtists = 8,
	TrackInfoArtwork = 16,
};

struct ListPlaylistTrackInfo {
	char trackInfoFlags;
	int playlist;
};

struct PlayTrackCommandInfo{
	int playlistId;
	int trackId;
};

union CommandInfo {
	char ListFlags;
	int playlist;
	ListPlaylistTrackInfo listPlaylistTrackInfo;
	PlayTrackCommandInfo playTrackCommandInfo;
};

class PlaylistTask: public Task {
public:
	PlaylistTask();
	PlaylistTask(TcpConnection* client, Command command);
	virtual ~PlaylistTask();

	void setCommand(Command command);
	Command getCommand();

	void setCommandInfo(CommandInfo info);
	CommandInfo getCommandInfo();

protected:
	Command command;
	CommandInfo info;
};

} /* namespace fambogie */

#endif /* PLAYLISTTASK_H_ */
