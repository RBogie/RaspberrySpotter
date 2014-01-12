/*
 * MessageConverter.cpp
 *
 *  Created on: Dec 27, 2013
 *      Author: rob
 */

#include <cstring>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "MessageConversion.hpp"
#include "TcpServer.hpp"

#include "Tasks/Task.hpp"
#include "Tasks/PlayerTask.hpp"
#include "Tasks/PlaylistTask.h"

#include "Responses/ClientResponse.hpp"
#include "Responses/ListResponse.hpp"
#include "Responses/PlayerResponse.hpp"
#include "Responses/ResponseStructures.hpp"
#include "Responses/StatusResponse.hpp"

#include "Base64Encoder/modp_b64.h"
using namespace rapidjson;

static int imageRequestIdCount = 0;

static void imageLoadedCallback(sp_image* image, void* userData) {
	int imageRequestId = *((int*) userData);
	delete (int*) userData;

	Document d;
	d.SetObject();

	Value type("ImageLoaded");
	d.AddMember("Type", type, d.GetAllocator());

	Value typeSpecific;
	typeSpecific.SetObject();

	Value imageId(imageRequestId);
	typeSpecific.AddMember("ImageId", imageId, d.GetAllocator());

	size_t size;
	const char* data = (const char*) sp_image_data(image, &size);
	char* b64Data = new char[modp_b64_encode_len(size)];

	int encodedSize = modp_b64_encode(b64Data, data, size);
	if (encodedSize > 0) {
		Value albumArt;
		albumArt.SetString(b64Data, d.GetAllocator());
		typeSpecific.AddMember("ImageData", albumArt, d.GetAllocator());
	}
	delete[] b64Data;

	d.AddMember("TypeSpecific", typeSpecific, d.GetAllocator());

	Value broadcast(true);
	d.AddMember("Broadcast", broadcast, d.GetAllocator());

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);
	const char* message = buffer.GetString();

	fambogie::TcpServer::getServerInstance()->broadcastMessage(message);

	return;
}

namespace fambogie {
namespace MessageConversion {

const char* getHandshakeInitiation() {
	return "{"
			"\"ServerName\": \"SpotterServer\","
			"\"ProtocolVersion\": 2"
			"}";
}

bool isHandshakeCorrect(const char* response) {
	Document d;
	d.Parse<0>(response);
	if (d.IsObject()) {
		if (d.HasMember("ClientName") && d.HasMember("ProtocolVersion")) {
			const Value& protocolVersion = d["ProtocolVersion"];
			return (protocolVersion.IsInt() && protocolVersion.GetInt() == 2);
		}
	}
	return false;
}

char* convertStatusResponseToJson(StatusResponse* response, bool broadcast) {
	Document d;
	d.SetObject();

	Value type("Status");
	d.AddMember("Type", type, d.GetAllocator());

	Value typeSpecific;
	typeSpecific.SetObject();

	Value status(response->isSuccess() ? "Ok" : "Error");
	typeSpecific.AddMember("Status", status, d.GetAllocator());
	const char* msg = response->getMessage();
	if (msg != nullptr) {
		Value message(msg);
		typeSpecific.AddMember("Message", message, d.GetAllocator());
	}

	d.AddMember("TypeSpecific", typeSpecific, d.GetAllocator());

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);
	const char* message = buffer.GetString();
	char* newMessage = new char[buffer.Size() + 1];
	memcpy(newMessage, message, buffer.Size());
	newMessage[buffer.Size()] = '\0';
	return newMessage;
}

Task* convertJsonToTask(const char* json) {
	Document d;
	d.Parse<0>(json);
	if (d.IsObject() && d.HasMember("Type") && d.HasMember("TypeSpecific")) {
		const char* type = d["Type"].GetString();
		const Value& typeSpecific = d["TypeSpecific"];
		if (typeSpecific.IsObject()) {
			if (strcasecmp(type, "Playlist") == 0) {
				if (typeSpecific.HasMember("Command")) {
					PlaylistTask* task = new PlaylistTask();
					const char* command = typeSpecific["Command"].GetString();
					CommandInfo commandInfo;
					if (strcasecmp(command, "List") == 0) {
						task->setCommand(CommandList);
						commandInfo.ListFlags = 0;
						if (typeSpecific.HasMember("CommandInfo")) {
							const Value& info = typeSpecific["CommandInfo"];
							if (info.IsArray()) {
								for (SizeType i = 0; i < info.Size(); i++) {
									const char* temp = info[i].GetString();
									if (strcasecmp(temp, "Name") == 0) {
										commandInfo.ListFlags |= Name;
									}
									if (strcasecmp(temp, "NumTracks") == 0) {
										commandInfo.ListFlags |= NumTracks;
									}
									if (strcasecmp(temp, "Description") == 0) {
										commandInfo.ListFlags |= Description;
									}
									if (strcasecmp(temp, "Image") == 0) {
										commandInfo.ListFlags |= Image;
									}
								}

							}
						}
					} else if (strcasecmp(command, "PlayPlaylist") == 0) {
						task->setCommand(CommandPlayPlaylist);
						if (typeSpecific.HasMember("PlaylistId")) {
							const Value& info = typeSpecific["PlaylistId"];
							if (info.IsInt()) {
								commandInfo.playlist = info.GetInt();
							} else {
								logError(
										"convertJsonToTask: incorrect PlaylistID");
							}
						} else {
							logError("convertJsonToTask: missing PlaylistID");
						}
					} else {
						logError(
								"convertJsonToTask: Unknown (playlist) command!");
						delete task;
						return nullptr;
					}
					task->setCommandInfo(commandInfo);
					return dynamic_cast<Task*>(task);
				} else {
					logError(
							"convertJsonToTask: No playlist command available!");
					return nullptr;
				}
			} else if (strcasecmp(type, "Player") == 0) {
				if (typeSpecific.HasMember("Command")) {
					PlayerTask* task = new PlayerTask();
					const char* command = typeSpecific["Command"].GetString();
					if (strcasecmp(command, "Play") == 0) {
						task->setCommand(PlayerCommandPlay);
					} else if (strcasecmp(command, "Pause") == 0) {
						task->setCommand(PlayerCommandPause);
					} else if (strcasecmp(command, "Seek") == 0) {
						task->setCommand(PlayerCommandSeek);
						PlayerCommandInfo info;
						info.seekPosition = 0;
						if (typeSpecific.HasMember("SeekPosition")
								& typeSpecific["SeekPosition"].IsInt()) {
							info.seekPosition =
									typeSpecific["SeekPosition"].GetInt();
							if (info.seekPosition < 0) {
								info.seekPosition = 0;
							}
						}
						task->setCommandInfo(info);
					} else if (strcasecmp(command, "CurrentPlayingInfo") == 0) {
						task->setCommand(PlayerCommandCurrentPlayingInfo);
					} else {
						logError(
								"convertJsonToTask: Unknown (player) command!");
						delete task;
						return nullptr;
					}
					return task;
				} else {
					logError("convertJsonToTask: No command supplied");
				}
			} else {
				logError("convertJsonToTask: Unknown type!");
				return nullptr;
			}
		} else {
			logError("convertJsonToTask: TypeSpecific must be an object");
			return nullptr;
		}
	} else {
		logError("convertJsonToTask: Type information missing!");
		return nullptr;
	}
}

char* convertListPlaylistInfoToJson(ListResponse<PlaylistInfo*>* response,
		bool broadcast) {
	Document d;
	d.SetObject();
	Value type("List");
	d.AddMember("Type", type, d.GetAllocator());
	Value typeSpecific;
	typeSpecific.SetObject();

	Value listType("PlaylistInfo");
	typeSpecific.AddMember("ListType", listType, d.GetAllocator());
	Value list;
	list.SetArray();
	while (response->getListSize() > 0) {
		Value playlistInfo;
		playlistInfo.SetObject();

		bool infoAvailable = false;

		PlaylistInfo* info = response->removeFirstMember();

		if (info->id > -1) {
			infoAvailable = true;
			Value id(info->id);
			playlistInfo.AddMember("Id", id, d.GetAllocator());
		}
		if (info->name != nullptr) {
			infoAvailable = true;
			Value name(info->name);
			playlistInfo.AddMember("Name", name, d.GetAllocator());
		}
		if (info->numTracks > -1) {
			infoAvailable = true;
			Value numTracks(info->numTracks);
			playlistInfo.AddMember("NumTracks", numTracks, d.GetAllocator());
		}
		if (info->description != nullptr) {
			infoAvailable = true;
			Value description(info->description);
			playlistInfo.AddMember("Description", description,
					d.GetAllocator());
		}
		if (infoAvailable) {
			list.PushBack(playlistInfo, d.GetAllocator());
		}
		delete info;
	}
	typeSpecific.AddMember("List", list, d.GetAllocator());

	d.AddMember("TypeSpecific", typeSpecific, d.GetAllocator());

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);
	const char* message = buffer.GetString();
	char* newMessage = new char[buffer.Size() + 1];
	memcpy(newMessage, message, buffer.Size());
	newMessage[buffer.Size()] = '\0';
	return newMessage;
}

char* convertPlayerResponseToJson(PlayerResponse* response, bool broadcast) {
	Document d;
	d.SetObject();
	Value type("Player");
	d.AddMember("Type", type, d.GetAllocator());
	Value broadcastField(broadcast);
	d.AddMember("Broadcast", broadcastField, d.GetAllocator());
	Value typeSpecific;
	typeSpecific.SetObject();

	Value playerResponseType("TrackInfo");
	typeSpecific.AddMember("PlayerResponseType", playerResponseType,
			d.GetAllocator());

	Value trackplaying(response->isCurrentlyPlaying());
	typeSpecific.AddMember("TrackPlaying", trackplaying, d.GetAllocator());

	Value tracks;
	tracks.SetArray();
	TrackInfo* trackInfo = response->getPlayerResponseInfo().trackInfo;
	while (trackInfo != nullptr) {
		Value track;
		track.SetObject();

		if (trackInfo->name != nullptr) {
			Value name(trackInfo->name);
			track.AddMember("Name", name, d.GetAllocator());
		}

		if (trackInfo->duration != nullptr) {
			Value name(trackInfo->duration);
			track.AddMember("Duration", name, d.GetAllocator());
		}

		if (trackInfo->numArtists > 0) {
			Value artists;
			artists.SetArray();
			for (int i = 0; i < trackInfo->numArtists; i++) {
				Value artist(trackInfo->artists[i]);
				artists.PushBack(artist, d.GetAllocator());
			}
			track.AddMember("Artists", artists, d.GetAllocator());
		}

		if (trackInfo->albumName != nullptr) {
			Value albumName(trackInfo->albumName);
			track.AddMember("AlbumName", albumName, d.GetAllocator());
		}

		if (trackInfo->albumArt != nullptr) {
			if (sp_image_is_loaded(trackInfo->albumArt)) {
				size_t size;
				const char* data = static_cast<const char*>(sp_image_data(
						trackInfo->albumArt, &size));

				if (data != nullptr && size > 0) {
					char* b64Data = new char[modp_b64_encode_len(size)];
					int encodedSize = modp_b64_encode(b64Data, data, size);
					if (encodedSize > 0) {
						Value albumArt;
						albumArt.SetString(b64Data, d.GetAllocator());
						track.AddMember("AlbumArt", albumArt, d.GetAllocator());
					}
					delete[] b64Data;
				}
			} else {
				int* imageRequestId = new int;
				*imageRequestId = imageRequestIdCount++;
				Value albumArtImageId(*imageRequestId);
				track.AddMember("AlbumArtImageId", albumArtImageId,
						d.GetAllocator());
				sp_image_add_load_callback(trackInfo->albumArt,
						&imageLoadedCallback, (void*) imageRequestId);
			}
		}

		tracks.PushBack(track, d.GetAllocator());
		trackInfo = trackInfo->nextTrack;
	}
	typeSpecific.AddMember("Tracks", tracks, d.GetAllocator());

	d.AddMember("TypeSpecific", typeSpecific, d.GetAllocator());

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);
	const char* message = buffer.GetString();
	char* newMessage = new char[buffer.Size() + 1];
	memcpy(newMessage, message, buffer.Size());
	newMessage[buffer.Size()] = '\0';
	return newMessage;
}

char* convertResponseToJson(ClientResponse* response, bool broadcast) {
	switch (response->getResponseType()) {
	case ClientResponse::List: {
		//Make a static cast first, to an incorrect type, just to get the type out of it #UGLYHACK
		ListResponse<char>* listResponse =
				static_cast<ListResponse<char>*>(response);
		switch (listResponse->getListType()) {
		case ListTypePlaylist:
			return convertListPlaylistInfoToJson(
					dynamic_cast<ListResponse<PlaylistInfo*>*>(response),
					broadcast);
			break;
		default:
			logError("Unknown listtype");
		}

		break;
	}
		break;
	case ClientResponse::Player:
		return convertPlayerResponseToJson(
				dynamic_cast<PlayerResponse*>(response), broadcast);
		break;
	case ClientResponse::Status:
		return convertStatusResponseToJson(
				dynamic_cast<StatusResponse*>(response), broadcast);
	default:
		logError("Unknown ClientResponse type");
	}
	delete response;
	return nullptr;
}

} /* namespace MessageConversion */
} /* namespace fambogie */
