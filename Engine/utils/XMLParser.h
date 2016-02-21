#ifndef INC_XMLPARSER_
#define INC_XMLPARSER_

#pragma warning (disable : 4786 )

#include <string>
#include <map>
#include "mcv_platform.h"

class MKeyValue : public std::map<std::string, std::string> {
	void writeAttributes(std::ostream &os) const;
public:
	void   put(const char *what, int value);
	void   put(const char *what, float value);
	void   put(const char *what, bool value);
	void   writeSingle(std::ostream &os, const char *what) const;
	void   writeStartElement(std::ostream &os, const char *what) const;
	void   writeEndElement(std::ostream &os, const char *what) const;

	int    getInt(const char *what, int default_value) const;
	float  getFloat(const char *what, float default_value) const;
	bool   getBool(const char *what, bool default_value) const;
	void   getMatrix(const char *what, MAT44 &target) const;
	VEC3   getPoint(const char *what) const;
	CQuaternion   getQuat(const char *what) const;
	std::string getString(const std::string &what, const std::string default_value) const;
};

class CXMLParser {
	// Static for interface with XML
	static void xmlStartElement(void *userData, const char *elem, const char **atts);
	static void xmlData(void *userData, const char *data, int length);
	static void xmlEndElement(void *userData, const char *elem);

protected:
	std::string  xml_error;

public:
	virtual void onStartElement(const std::string &elem, MKeyValue &atts);
	virtual void onData(const std::string &data);
	virtual void onEndElement(const std::string &elem);
	virtual ~CXMLParser() { }

	bool xmlParseFile(const std::string &filename);
	bool xmlParseStream(std::istream &is, const char *stream_name = NULL);
	const std::string &getXMLError() const { return xml_error; }
};

#endif 