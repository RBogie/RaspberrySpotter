/*
 * StatusResponse.hpp
 *
 *  Created on: Dec 29, 2013
 *      Author: rob
 */

#ifndef STATUSRESPONSE_HPP_
#define STATUSRESPONSE_HPP_

namespace fambogie {

class StatusResponse: public ClientResponse {
public:
	StatusResponse(bool success, const char* msg) :
			ClientResponse(Status) {
		this->success = success;
		this->msg = msg;
	}
	virtual ~StatusResponse() {
	}

	bool isSuccess() { return success; }
	void setSuccess(bool success) { this->success=success; }
	const char* getMessage() { return msg; }
	void setMessage(const char* msg) {	this->msg = msg; }

private:
	bool success;
	const char* msg;
};

} /* namespace fambogie */

#endif /* STATUSRESPONSE_HPP_ */
