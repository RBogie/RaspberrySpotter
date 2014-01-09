// Java-Style Properties in C++
//
// (c) Paul D. Senzee
// Senzee 5
// http://senzee.blogspot.com
//
// Consent given in the following blogpost, since this software is
// not commercial: http://www.paulsenzee.com/2008/07/code-use-policy.html

#ifndef _PROPERTYUTIL_H
#define _PROPERTYUTIL_H

#include <map>
#include <string>
#include <iostream>

class PropertyException : std::exception {
public:
	PropertyException(const char* message) {
		this->message = message;
	}

	const char* what() {
		return message;
	}

private:
	const char* message;
};

class PropertyUtil
{
    enum { DEBUG = 0 };

public:

    typedef std::map<std::string, std::string> PropertyMapT;
    typedef PropertyMapT::value_type           value_type;
    typedef PropertyMapT::iterator             iterator;

    static void read(const char *filename, PropertyMapT &map);
    static void read(std::istream &is, PropertyMapT &map);
    static void write(const char *filename, PropertyMapT &map, const char *header = NULL);
    static void write(std::ostream &os, PropertyMapT &map, const char *header = NULL);
    static void print(std::ostream &os, PropertyMapT &map);

private:

    static inline char m_hex(int nibble)
    {
        static const char *digits = "0123456789ABCDEF";
        return digits[nibble & 0xf];
    }
};

#endif  // _PROPERTYUTIL_H
