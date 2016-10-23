#include "mcv_platform.h"
#include "text_encode.h"

#define MAX_STRING_SIZE 1024
int GetUtf8CharacterLength(unsigned char utf8Char)
{
	if (utf8Char < 0x80) return 1;
	else if ((utf8Char & 0x20) == 0) return 2;
	else if ((utf8Char & 0x10) == 0) return 3;
	else if ((utf8Char & 0x08) == 0) return 4;
	else if ((utf8Char & 0x04) == 0) return 5;

	return 6;
}

char Utf8ToLatin1Character(char *s, int *readIndex)
{
	int len = GetUtf8CharacterLength(static_cast<unsigned char>(s[*readIndex]));
	if (len == 1)
	{
		char c = s[*readIndex];
		(*readIndex)++;

		return c;
	}

	unsigned int v = (s[*readIndex] & (0xff >> (len + 1))) << ((len - 1) * 6);
	(*readIndex)++;
	for (len--; len > 0; len--)
	{
		v |= (static_cast<unsigned char>(s[*readIndex]) - 0x80) << ((len - 1) * 6);
		(*readIndex)++;
	}

	return (v > 0xff) ? 0 : (char)v;
}

void TextEncode::Utf8ToLatin1String(char* s)
{
	for (int readIndex = 0, writeIndex = 0; ; writeIndex++)
	{
		if (s[readIndex] == 0)
		{
			s[writeIndex] = 0;
			break;
		}

		char c = Utf8ToLatin1Character(s, &readIndex);
		if (c == 0)
		{
			c = '_';
		}
		assert(writeIndex < MAX_STRING_SIZE);
		s[writeIndex] = c;
	}
}

std::string TextEncode::Utf8ToLatin1String(const char* text)
{
	char buffer[MAX_STRING_SIZE];
	sprintf(buffer, "%s", text);
	Utf8ToLatin1String(buffer);

	return std::string(buffer);
}

std::string TextEncode::Latin1ToUtf8String(const char* text)
{
	char in[MAX_STRING_SIZE];
	char out[MAX_STRING_SIZE * 2];
	sprintf(in, "%s", text);
	Latin1ToUtf8((unsigned char*)in, (unsigned char*)out);

	return std::string(out);
}

void TextEncode::Latin1ToUtf8(unsigned char* in, unsigned char* out)
{
	int i = 0;
	while (*in) {
		assert(i++ < MAX_STRING_SIZE);
		if ((*in) < 128) *out++ = *in++;
		else *out++ = 0xc2 + (*in > 0xbf), *out++ = (*in++ & 0x3f) + 0x80;
	}
	*out = 0;
}