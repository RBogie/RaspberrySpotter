/*
 * ListResponse.hpp
 *
 *  Created on: Dec 28, 2013
 *      Author: rob
 */

#ifndef LISTRESPONSE_HPP_
#define LISTRESPONSE_HPP_

#include "ClientResponse.hpp"
#include "list"
namespace fambogie {

enum ListType {
		ListTypeUnknown,
		ListTypePlaylist,
		ListTypeSong,
	};

template<class T>
class ListResponse : public fambogie::ClientResponse {
public:

	ListResponse() : ClientResponse(List), listType(ListTypeUnknown){}
	virtual ~ListResponse(){}

	ListType getListType() {return listType;}
	void setListType(ListType type) {this->listType = type;}

	void addMember(T member) {
		this->list.push_back(member);
	}

	T removeFirstMember() {
		T t = this->list.front();
		this->list.pop_front();
		return t;
	}

	int getListSize() {
		return list.size();
	}

private:
	ListType listType;
	std::list<T> list;

};

} /* namespace fambogie */

#endif /* LISTRESPONSE_HPP_ */
