/*
 * ClientResponse.hpp
 *
 *  Created on: Dec 28, 2013
 *      Author: rob
 */

#ifndef CLIENTRESPONSE_HPP_
#define CLIENTRESPONSE_HPP_

namespace fambogie {

class ClientResponse {
public:

	enum Type {
		Unknown,
		Status,
		List,
		Player,
	};

	ClientResponse();
	ClientResponse(Type responseType);
	virtual ~ClientResponse();

	void setResponseType(Type responseType);
	Type getResponseType();
protected:
	Type responseType;
};

} /* namespace fambogie */

#endif /* CLIENTRESPONSE_HPP_ */
