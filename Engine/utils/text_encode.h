#ifndef INC_TEXT_ENCODE_H_
#define INC_TEXT_ENCODE_H_

namespace TextEncode {
	void Utf8ToLatin1String(char *s);
	std::string Utf8ToLatin1String(const char *s);
	void Latin1ToUtf8(unsigned char* in, unsigned char* out);
	std::string Latin1ToUtf8String(const char *s);
};

#endif