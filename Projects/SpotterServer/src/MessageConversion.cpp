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
	if(d.IsObject()) {
		if(d.HasMember("ClientName") && d.HasMember("ProtocolVersion")) {
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
	if(error && msg != nullptr) {
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
	if(d.IsObject() && d.HasMember("Type")) {
		const char* type = d["Type"].GetString();
		if(strcasecmp(type, "Playlist") == 0) {
			if(d.HasMember("Command")) {
				PlaylistTask* task = new PlaylistTask();
				const char* command = d["Command"].GetString();
				if(strcasecmp(command, "List") == 0) {
					task->setCommand(CommandList);
				} else {
					logError("convertJsonToTask: Unknown (playlist) command!");
				}
				return dynamic_cast<Task*>(task);
			} else {
				logError("convertJsonToTask: No playlist command available!");
				return nullptr;
			}
		} else {
			logError("convertJsonToTask: Unknown type!");
			return nullptr;
		}
	} else {
		logError("convertJsonToTask: No type specified!");
		return nullptr;
	}
}

} /* namespace MessageConversion */
} /* namespace fambogie */
