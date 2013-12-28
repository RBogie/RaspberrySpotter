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
#include "Tasks/PlaylistTask.h"

#include "Responses/ListResponse.hpp"

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

char* handshakeStatusToJson(bool error, const char* msg) {
	Document d;
	d.SetObject();
	Value status(error ? "Error" : "Ok");
	d.AddMember("Status", status, d.GetAllocator());
	Value errorMsg;
	if (error && msg != nullptr) {
		errorMsg = msg;
		d.AddMember("ErrorMessage", errorMsg, d.GetAllocator());
	}
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);
	int size = buffer.Size();
	const char* message = buffer.GetString();
	char* newMessage = new char[buffer.Size()];
	memcpy(newMessage, message, buffer.Size());
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
					if (strcasecmp(command, "List") == 0) {
						task->setCommand(CommandList);
						if (typeSpecific.HasMember("CommandInfo")) {
							const Value& info = typeSpecific["CommandInfo"];
							if (info.IsArray()) {
								CommandInfo commandInfo;
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
								task->setCommandInfo(commandInfo);
							}
						}
					} else {
						logError(
								"convertJsonToTask: Unknown (playlist) command!");
					}
					return dynamic_cast<Task*>(task);
				} else {
					logError(
							"convertJsonToTask: No playlist command available!");
					return nullptr;
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
			playlistInfo.AddMember("NumTracks", numTracks,
					d.GetAllocator());
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
	char* newMessage = new char[buffer.Size()];
	memcpy(newMessage, message, buffer.Size());
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
	}
		break;
	default:
		logError("Unknown ClientResponse type");
	}
	delete response;
	return nullptr;
}

} /* namespace MessageConversion */
} /* namespace fambogie */
