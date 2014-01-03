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
#include "Tasks/Task.hpp"
#include "Tasks/PlayerTask.hpp"
#include "Tasks/PlaylistTask.h"

#include "Responses/ClientResponse.hpp"
#include "Responses/ListResponse.hpp"
#include "Responses/ResponseStructures.hpp"
#include "Responses/StatusResponse.hpp"

using namespace rapidjson;

namespace fambogie {
namespace MessageConversion {

const char* getHandshakeInitiation() {
	return "{"
			"ServerName\": \"SpotterServer\","
			"\"ProtocolVersion\": 1"
			"}";
}

bool isHandshakeCorrect(const char* response) {
	Document d;
	d.Parse<0>(response);
	if (d.IsObject()) {
		if (d.HasMember("ClientName") && d.HasMember("ProtocolVersion")) {
			const Value& protocolVersion = d["ProtocolVersion"];
			return (protocolVersion.IsInt() && protocolVersion.GetInt() == 1);
		}
	}
	return false;
}

char* convertStatusResponseToJson(StatusResponse* response) {
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
	int size = buffer.Size();
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
						if(typeSpecific.HasMember("SeekPosition") & typeSpecific["SeekPosition"].IsInt()) {
							info.seekPosition = typeSpecific["SeekPosition"].GetInt();
							if(info.seekPosition < 0) {
								info.seekPosition = 0;
							}
						}
						task->setCommandInfo(info);
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

char* convertListPlaylistInfoToJson(ListResponse<PlaylistInfo*>* response) {
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
	for (int i = 0; i < response->getListSize(); i++) {
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
	int size = buffer.Size();
	const char* message = buffer.GetString();
	char* newMessage = new char[buffer.Size() + 1];
	memcpy(newMessage, message, buffer.Size());
	newMessage[buffer.Size()] = '\0';
	return newMessage;
}

char* convertResponseToJson(ClientResponse* response) {
	switch (response->getResponseType()) {
	case ClientResponse::List: {
		//Make a static cast first, to an incorrect type, just to get the type out of it
		ListResponse<char>* listResponse =
				static_cast<ListResponse<char>*>(response);
		switch (listResponse->getListType()) {
		case ListTypePlaylist:
			return convertListPlaylistInfoToJson(
					dynamic_cast<ListResponse<PlaylistInfo*>*>(response));
			break;
		default:
			logError("Unknown listtype");
		}

		break;
	}
		break;
	case ClientResponse::Status:
		return convertStatusResponseToJson(
				dynamic_cast<StatusResponse*>(response));
	default:
		logError("Unknown ClientResponse type");
	}
	delete response;
	return nullptr;
}

} /* namespace MessageConversion */
} /* namespace fambogie */
