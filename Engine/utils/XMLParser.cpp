#pragma warning( disable : 4786 )

#include "mcv_platform.h"
#include <cstdio>
#include <cassert>
#include "XMLParser.h"
#include <sstream>
#include <fstream>
#define XML_STATIC
#include "expat/expat.h"
#include "utils.h"

using namespace DirectX;

/*------------------------------------------------------------------
|
\------------------------------------------------------------------*/
std::string decodeFromUTF8(const char *data, size_t nMax) {
	const unsigned char *szSource = (const unsigned char *)data;

	std::string sFinal;
	sFinal.reserve(nMax);

	size_t n;
	for (n = 0; n < nMax; ++n) {
		unsigned char z = szSource[n];
		if (z < 127) {
			sFinal += (TCHAR)z;
		}
		else if (z >= 192 && z <= 223)
		{
			assert(n < nMax - 1);
			// character is two bytes
			if (n >= nMax - 1)
				break; // something is wrong
			unsigned char y = szSource[n + 1];
			sFinal += (TCHAR)((z - 192) * 64 + (y - 128));
			n = n + 1;
		}
		else if (z >= 224 && z <= 239)
		{
			// character is three bytes
			assert(n < nMax - 2);
			if (n >= nMax - 2)
				break; // something is wrong
			unsigned char y = szSource[n + 1];
			unsigned char x = szSource[n + 2];
			sFinal += (TCHAR)((z - 224) * 4096 + (y - 128) * 64 + (x - 128));
			n = n + 2;
		}
		else if (z >= 240 && z <= 247)
		{
			// character is four bytes
			assert(n < nMax - 3);
			if (n >= nMax - 3)
				break; // something is wrong
			unsigned char y = szSource[n + 1];
			unsigned char x = szSource[n + 2];
			unsigned char w = szSource[n + 3];
			sFinal += (TCHAR)((z - 240) * 262144 + (y - 128) * 4096 +
				(x - 128) * 64 + (w - 128));
			n = n + 3;
		}
		else if (z >= 248 && z <= 251)
		{
			// character is four bytes
			assert(n < nMax - 4);
			if (n >= nMax - 4)
				break; // something is wrong
			unsigned char y = szSource[n + 1];
			unsigned char x = szSource[n + 2];
			unsigned char w = szSource[n + 3];
			unsigned char v = szSource[n + 4];
			sFinal += (TCHAR)((z - 248) * 16777216 + (y - 128) * 262144 +
				(x - 128) * 4096 + (w - 128) * 64 + (v - 128));
			n = n + 4;
		}
		else if (z >= 252 && z <= 253)
		{
			// character is five bytes
			assert(n < nMax - 5);
			if (n >= nMax - 5)
				break; // something is wrong
			unsigned char y = szSource[n + 1];
			unsigned char x = szSource[n + 2];
			unsigned char w = szSource[n + 3];
			unsigned char v = szSource[n + 4];
			unsigned char u = szSource[n + 5];
			sFinal += (TCHAR)((z - 252) * 1073741824 + (y - 128) * 16777216 +
				(x - 128) * 262144 + (w - 128) * 4096 + (v - 128) * 64 + (u - 128));
			n = n + 5;
		}
	}
	return sFinal;
}

/*------------------------------------------------------------------
|
\------------------------------------------------------------------*/
std::string MKeyValue::getString(const std::string &what, const std::string default_value) const {
	const_iterator it = find(what);
	if (it == end())
		return default_value;
	return decodeFromUTF8(it->second.c_str(), strlen(it->second.c_str()));
}
bool MKeyValue::has(const char* what) const {
	return find(what) != end();
}
/*------------------------------------------------------------------
|
\------------------------------------------------------------------*/
int    MKeyValue::getInt(const char *what, int default_value) const {
	const_iterator it = find(what);

	if (it == end())
		return default_value;
	return atoi((*it).second.c_str());
}

template < class T >
void putKey(MKeyValue &k, const char *what, T value) {
	/*std::ostrstream oss;
	oss << value << '\0';
  char *buf = oss.str( );
	k[ what ] = buf;
  delete buf;*/
  // Nos libramos del delete?
	std::ostringstream oss;
	oss << value;
	const std::string &buf = oss.str();
	k[what] = buf;
}

//write VEC3
void putPoint(MKeyValue &k, const char *what, VEC3 data) {
	/*std::ostrstream oss;
	oss << value << '\0';
	char *buf = oss.str( );
	k[ what ] = buf;
	delete buf;*/
	// Nos libramos del delete?
	std::string value = std::to_string(data.x) + " " + std::to_string(data.y) + " " + std::to_string(data.z);
	std::ostringstream oss;
	oss << value;
	const std::string &buf = oss.str();
	k[what] = buf;
}

//write VEC4
void putQuat(MKeyValue &k, const char *what, VEC4 data) {
	/*std::ostrstream oss;
	oss << value << '\0';
	char *buf = oss.str( );
	k[ what ] = buf;
	delete buf;*/
	// Nos libramos del delete?
	std::string value = std::to_string(data.x) + " " + std::to_string(data.y) + " " + std::to_string(data.z) + " " + std::to_string(data.w);
	std::ostringstream oss;
	oss << value;
	const std::string &buf = oss.str();
	k[what] = buf;
}

void MKeyValue::put(const char *what, int value) {
	putKey(*this, what, value);
}

void MKeyValue::put(const char *what, VEC3 value) {
	putPoint(*this, what, value);
}
void MKeyValue::put(const char *what, bool value) {
	putKey(*this, what, value);
}
void MKeyValue::put(const char *what, float value) {
	putKey(*this, what, value);
}

void MKeyValue::put(const char *what, VEC4 value) {
	putQuat(*this, what, value);
}

float MKeyValue::getFloat(const char *what, float default_value) const {
	const_iterator it = find(what);

	if (it == end())
		return default_value;
	return (float)atof((*it).second.c_str());
}

bool MKeyValue::getBool(const char *what, bool default_value) const {
	const_iterator it = find(what);
	if (it == end())
		return default_value;
	// Check agains valid keywords
	const char *yes[] = { "1", "yes", "YES", "Yes", "true", "TRUE", "True" };
	int i = 0;
	for (i = 0; i < sizeof(yes) / sizeof(yes[0]); ++i)
		if ((*it).second == yes[i])
			return true;
	return false;
}

VEC3 MKeyValue::getPoint(const char *what) const {
	const_iterator it = find(what);
	if (it == end())
		return XMVectorSet(0, 0, 0, 0);
	// it tiene el string "1.2 5.4 3.2"
	const char *values = it->second.c_str();
	float x, y, z;
	int n = sscanf(values, "%f %f %f", &x, &y, &z);
	assert(n == 3);
	return VEC3(x, y, z);
}

CQuaternion MKeyValue::getQuat(const char *what) const {
	const_iterator it = find(what);
	if (it == end())
		return XMVectorSet(0, 0, 0, 1);
	const char *values = it->second.c_str();
	float x, y, z, w;
	int n = sscanf(values, "%f %f %f %f", &x, &y, &z, &w);
	assert(n == 4);
	return CQuaternion(x, y, z, w);
}

void MKeyValue::getMatrix(const char *what, MAT44 &target) const {
	/*
	  const_iterator it = find (what);
	  assert( it != end() || fatal( "Can't find matrix attribute %s\n", what ));
	  const char *values = it->second.c_str();
	  int n = sscanf( values,
		  "%f %f %f "
		  "%f %f %f "
		  "%f %f %f "
		  "%f %f %f"
	  , &target.m[0][0]
	  , &target.m[0][1]
	  , &target.m[0][2]
	  , &target.m[1][0]
	  , &target.m[1][1]
	  , &target.m[1][2]
	  , &target.m[2][0]
	  , &target.m[2][1]
	  , &target.m[2][2]
	  , &target.m[3][0]
	  , &target.m[3][1]
	  , &target.m[3][2]
	  );
	  assert( n == 12 || fatal( "Can't read 12 floats from matrix attribute %s. only %d\n", what, n ));
	  target.m[0][3] = 0.0f;
	  target.m[1][3] = 0.0f;
	  target.m[2][3] = 0.0f;
	  target.m[3][3] = 1.0f;
	*/
}

void MKeyValue::writeSingle(std::ostream &os, const char *what) const {
	os << "<" << what << "\n";
	writeAttributes(os);
	os << "\t/>\n";
}

void MKeyValue::writeAttributes(std::ostream &os) const {
	const_iterator i = begin();
	while (i != end()) {
		os << "\t" << i->first << "=\"" << i->second << "\"\n";
		++i;
	}
	auto a = const_cast<MKeyValue*>(this);
	a->clear();
}

void MKeyValue::writeStartElement(std::ostream &os, const char *what) const {
	os << "<" << what << "\n";
	writeAttributes(os);
	os << ">\n";
}

void MKeyValue::writeEndElement(std::ostream &os, const char *what) const {
	os << "</" << what << ">\n";
}

/*-<==>-----------------------------------------------------------------
/ Real xml handlers
/----------------------------------------------------------------------*/
void CXMLParser::onStartElement(const std::string &elem, MKeyValue &atts) {
	(void)elem;
	(void)atts;
}

void CXMLParser::onData(const std::string &data) {
	(void)data;
}

void CXMLParser::onEndElement(const std::string &elem) {
	(void)elem;
}

/*------------------------------------------------------------------
|
\------------------------------------------------------------------*/
void CXMLParser::xmlStartElement(void *userData, const char *element, const char **atts) {
	CXMLParser *self = (CXMLParser *)userData;
	// Convert the array of char pointers to a map
	MKeyValue map_atts;
	for (const char **p = atts; *p; p += 2) {
		const char *key = *p;
		const char *value = *(p + 1);
		map_atts[key] = value;
	}
	self->onStartElement(std::string(element), map_atts);
}

void CXMLParser::xmlData(void *userData, const char *data, int length) {
	CXMLParser *self = (CXMLParser *)userData;

	self->onData(decodeFromUTF8(data, length));
}

void CXMLParser::xmlEndElement(void *userData, const char *element) {
	CXMLParser *self = (CXMLParser *)userData;
	self->onEndElement(std::string(element));
}

/*------------------------------------------------------------------
|
\------------------------------------------------------------------*/
bool CXMLParser::xmlParseFile(const std::string &filename) {
	std::ifstream is(filename.c_str());
	if (!is.is_open()) {
		xml_error = "XML File " + filename + " not found";
		assert(false);
		return false;
	}
	return xmlParseStream(is, filename.c_str());
}

/*------------------------------------------------------------------
|
\------------------------------------------------------------------*/
#undef BUFSIZ
#define BUFSIZ 8192

bool CXMLParser::xmlParseStream(std::istream &is, const char *stream_name) {
	// Clear error msg
	char buf[BUFSIZ];
	xml_error = "";

	XML_Parser parser = XML_ParserCreate(NULL);

	XML_SetUserData(parser, this);
	XML_SetElementHandler(parser, xmlStartElement, xmlEndElement);
	XML_SetCharacterDataHandler(parser, xmlData);
	bool done = false;

	do {
		is.read(buf, sizeof(buf));
		size_t len = is.gcount();
		done = len < sizeof(buf);
		if (!XML_Parse(parser, buf, (int)len, done)) {
			char msg[512];
			_snprintf(msg, sizeof(msg) - 1, "XML Parser error '%s' at line %d when processing input stream( %s )\n",
				XML_ErrorString(XML_GetErrorCode(parser)),
				XML_GetCurrentLineNumber(parser),
				stream_name ? stream_name : "<UNNAMED>"
			);
			xml_error = std::string(msg);
			done = false;
			assert(done);
			break;
		}
	} while (!done);

	XML_ParserFree(parser);
	return done;
}