/*
 * ClientResponse.cpp
 *
 *  Created on: Dec 28, 2013
 *      Author: rob
 */

#include "ClientResponse.hpp"

namespace fambogie {

ClientResponse::ClientResponse() {
	responseType = Unknown;
}

ClientResponse::ClientResponse(Type responseType) {
	this->responseType = responseType;
}

ClientResponse::~ClientResponse() {
}

void ClientResponse::setResponseType(Type responseType) {
	this->responseType = responseType;
}

ClientResponse::Type ClientResponse::getResponseType() {
	return responseType;
}

} /* namespace fambogie */
