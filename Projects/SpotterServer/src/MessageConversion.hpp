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
#include "Responses/StatusResponse.hpp"

namespace fambogie {

namespace MessageConversion {

const char* getHandshakeInitiation();
bool isHandshakeCorrect(const char* response);

Task* convertJsonToTask(const char* json);

char* convertResponseToJson(ClientResponse* response, bool broadcast = false);

}

} /* namespace fambogie */

#endif /* MESSAGECONVERTER_H_ */
