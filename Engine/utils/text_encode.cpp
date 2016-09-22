#include "mcv_platform.h"
#include "text_encode.h"
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

		s[writeIndex] = c;
	}
}

std::string TextEncode::Utf8ToLatin1String(const char* text)
{
	char buffer[512];
	sprintf(buffer, "%s", text);
	Utf8ToLatin1String(buffer);

	return std::string(buffer);
}