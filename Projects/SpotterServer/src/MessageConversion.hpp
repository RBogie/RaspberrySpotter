/*
 * MessageConverter.h
 *
 *  Created on: Dec 27, 2013
 *      Author: rob
 */

#ifndef MESSAGECONVERTER_H_
#define MESSAGECONVERTER_H_

#include "Tasks/Task.hpp"
#include "Responses/ClientResponse.hpp"

namespace fambogie {

namespace MessageConversion {

const char* getHandshakeInitiation();
bool isHandshakeCorrect(const char* response);
char* handshakeStatusToJson(bool error, const char* msg);

Task* convertJsonToTask(const char* json);

char* convertResponseToJson(ClientResponse* response);

}

} /* namespace fambogie */

#endif /* MESSAGECONVERTER_H_ */
