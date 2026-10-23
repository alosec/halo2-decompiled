// @flags /O2 /Gr
/* UNICODE_STRINGS.CPP: the game's unicode (UTF-16) strings: copying, appending,
   formatting, conversion to and from ASCII and UTF-8, character classes and
   where a line of text may break */

#include "unknown_11c920.h"
#include "unknown_13fd90.h"
#include <stdarg.h>
#include <string.h>
#include <wchar.h>
#include <xtl.h>

struct unicode_range
{
	word first;
	word last;
};

struct unicode_escape
{
	word character;
	long value;
};

/* when an unknown escape may next be reported (GetTickCount, once a minute) */
dword g_unicode_escape_warning_time;

/* appends source to destination, keeping the result below maximum_count characters */
// @retail 0x13fa70
word *unicode_string_append(word *destination, const word *source, long maximum_count)
{
	word *d = destination;

	while (*d)
	{
		d++;
		maximum_count--;
	}

	while (*source && maximum_count > 1)
	{
		*d++ = *source++;
		maximum_count--;
	}

	*d = 0;
	return destination;
}

/* copies at most maximum_count - 1 characters and terminates the copy */
// @retail 0x13fac0
void unicode_string_copy(word *destination, const word *source, long maximum_count)
{
	wcsncpy((wchar_t *)destination, (const wchar_t *)source, maximum_count - 1);
	destination[maximum_count - 1] = 0;
}

/* upper-cases the ASCII letters and the accented e and o in place */
// @retail 0x13fae0
word *unicode_string_upper(word *string, long maximum_count)
{
	word *s = string;

	while (*s && maximum_count >= 0)
	{
		word c = *s;

		if (c >= 'a' && c <= 'z')
		{
			c -= 0x20;
		}
		else if (c > 0x7f)
		{
			long value = c;
			switch (value)
			{
				case 0xe9: value = 0xc9; break;
				case 0xf3: value = 0xd3; break;
			}
			c = (word)value;
		}

		*s = (word)c;
		s++;
		maximum_count--;
	}

	return string;
}

/* _vsnwprintf into buffer; the result is not terminated when it fills buffer */
// @retail 0x13fb50
int unicode_string_vsnprintf(word *buffer, long maximum_count, const word *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	return _vsnwprintf((wchar_t *)buffer, maximum_count, (const wchar_t *)format, arguments);
}

/* formats into buffer, always terminated */
// @retail 0x13fb70
void unicode_string_snprintf(word *buffer, long maximum_count, const word *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	_vsnwprintf((wchar_t *)buffer, maximum_count - 1, (const wchar_t *)format, arguments);
	buffer[maximum_count - 1] = 0;
}

/* converts to ASCII, characters above 0x7f becoming '?' */
// @retail 0x13fb90
void unicode_string_to_ascii(const word *source, char *destination, long maximum_count)
{
	const word *s = source;
	char *d = destination;
	long count = maximum_count;

	while (count > 0)
	{
		word c = *s;

		if (count == 1)
		{
			*d = 0;
		}
		else if (c <= 0x7f)
		{
			*d = (char)c;
		}
		else
		{
			*d = '?';
		}

		s++;
		d++;
		count--;

		if (!c || count <= 0)
		{
			break;
		}
	}
}

/* the character an escape ('|' followed by character) stands for; found says
   whether character is a known escape */
// @retail 0x13fbd0
long unicode_escape_character_lookup(word character, bool *found)
{
	bool valid = false;
	unicode_escape table[] =
	{
		{ '|', '|' },
		{ 'l', 0xe405 },
		{ 'r', 0xe406 },
		{ 'c', 0xe407 },
		{ 'n', 0xd },
		{ 't', 9 },
		{ 0, 0 }
	};
	dword result;
	result = '|';

	for (long i = 0; table[i].character; ++i)
	{
		if (character == table[i].character)
		{
			valid = true;
			result = table[i].value;
			break;
		}
	}

	if (!valid && character)
	{
		unsigned long time = GetTickCount();
		if (time > g_unicode_escape_warning_time)
		{
			g_unicode_escape_warning_time = time + 60000;
		}
	}

	*found = valid;
	return result;
}

/* converts ASCII to unicode, characters above 0x7f becoming a white square */
// @retail 0x13fc90
void ascii_string_to_unicode(const char *source, word *destination, long maximum_count)
{
	word *d = destination;
	const char *s = source;
	long count = maximum_count;

	while (count > 0)
	{
		char c = *s;

		if (count == 1)
		{
			*d = 0;
		}
		else if ((byte)c <= 0x7f)
		{
			*d = (short)c;
		}
		else
		{
			*d = 0x25a1;
		}

		if (!*s)
		{
			break;
		}

		d++;
		s++;
		count--;
	}
}

/* whether character lies in one of range_count sorted ranges (binary search) */
// @retail 0x13fcd0
bool unicode_ranges_contain(long range_count, utf32 character, const unicode_range *ranges)
{
	const unicode_range *const *ranges_reference = &ranges;
	bool result = false;
	long low = 0;
	long high = range_count - 1;
	long c = character.value;
	const unicode_range *r = *ranges_reference;

	while (low <= high)
	{
		long mid = (low + high) / 2;

		if ((word)c < r[mid].first)
		{
			high = mid - 1;
		}
		else if ((word)c > r[mid].last)
		{
			low = mid + 1;
		}
		else
		{
			result = true;
			break;
		}
	}

	return result;
}

bool unicode_character_is_east_asian(utf32 character);
bool unicode_character_cannot_begin_line(utf32 character);
bool unicode_character_cannot_end_line(utf32 character);

/* the white space characters */
unicode_range const g_unicode_white_space_ranges[] =
{
	{ 0x9, 0xd },
	{ 0x20, 0x20 },
	{ 0x85, 0x85 },
	{ 0xa0, 0xa0 },
	{ 0x1680, 0x1680 },
	{ 0x180e, 0x180e },
	{ 0x2000, 0x200a },
	{ 0x2028, 0x2028 },
	{ 0x2029, 0x2029 },
	{ 0x202f, 0x202f },
	{ 0x205f, 0x205f },
	{ 0x3000, 0x3000 }
};

/* whether a line may break between two characters: after white space or
   around east asian characters, unless the punctuation forbids it */
// @retail 0x13fd20
bool unicode_line_break_allowed(utf32 previous, utf32 character)
{
	bool result = false;

	if (previous.value)
	{
		bool non_beginning = unicode_character_cannot_begin_line(character);
		bool non_ending = unicode_character_cannot_end_line(previous);
		if ((unicode_ranges_contain(sizeof(g_unicode_white_space_ranges) / sizeof(g_unicode_white_space_ranges[0]), previous, g_unicode_white_space_ranges) ||
			unicode_character_is_east_asian(previous) || unicode_character_is_east_asian(character)) &&
			!non_beginning && !non_ending)
		{
			result = true;
		}
	}

	return result;
}

/* whether character is an east asian one (hangul jamo, the CJK and hangul
   blocks, compatibility ideographs, full width forms), between which a line
   may break */
// @retail 0x13fd90
bool unicode_character_is_east_asian(utf32 character)
{
	long c = character.value;

	if ((c >= 0x1100 && c <= 0x11ff) ||
		(c >= 0x3000 && c <= 0xd7af) ||
		(c >= 0xf900 && c <= 0xfaff) ||
		(c >= 0xff00 && c <= 0xffdc))
	{
		return true;
	}

	return false;
}

/* the punctuation that may not begin a line */
// @retail 0x13fde0
bool unicode_character_cannot_begin_line(utf32 character)
{
	bool result = false;

	switch ((dword)character.value)
	{
		case 0x21: case 0x22: case 0x25: case 0x29: case 0x2c: case 0x2e: case 0x3a: case 0x3b:
		case 0x3f: case 0x5d: case 0x7d: case 0xb0: case 0xb7: case 0x2013: case 0x2014: case 0x2019:
		case 0x201d: case 0x2022: case 0x2026: case 0x2027: case 0x2032: case 0x2033: case 0x2103: case 0x3001:
		case 0x3002: case 0x3009: case 0x300b: case 0x300d: case 0x300f: case 0x3011: case 0x3015: case 0x301e:
		case 0x3041: case 0x3043: case 0x3045: case 0x3047: case 0x3049: case 0x3063: case 0x3083: case 0x3085:
		case 0x3087: case 0x308e: case 0x30a1: case 0x30a3: case 0x30a5: case 0x30a7: case 0x30a9: case 0x30c3:
		case 0x30e3: case 0x30e5: case 0x30e7: case 0x30ee: case 0x30fc: case 0xfe30: case 0xfe50: case 0xfe51:
		case 0xfe52: case 0xfe54: case 0xfe55: case 0xfe56: case 0xfe57: case 0xfe5a: case 0xfe5c: case 0xfe5e:
		case 0xff01: case 0xff05: case 0xff09: case 0xff0c: case 0xff0e: case 0xff1a: case 0xff1b: case 0xff1f:
		case 0xff3d: case 0xff5d: case 0xff61: case 0xff64: case 0xff70: case 0xff9e: case 0xff9f: case 0xffe0:
		result = true;
		break;
	}

	return result;
}

/* the punctuation that may not end a line */
// @retail 0x140260
bool unicode_character_cannot_end_line(utf32 character)
{
	bool result = false;

	switch ((dword)character.value)
	{
		case 0x22: case 0x24: case 0x28: case 0x5b: case 0x5c: case 0x7b: case 0x2018: case 0x201c:
		case 0x2035: case 0x3008: case 0x300a: case 0x300c: case 0x300e: case 0x3010: case 0x3014: case 0x301d:
		case 0xfe59: case 0xfe5b: case 0xfe5d: case 0xff04: case 0xff08: case 0xff3b: case 0xff5b: case 0xffe1:
		case 0xffe5: case 0xffe6:
		result = true;
		break;
	}

	return result;
}

/* whether character is in the private use area (the game's own glyphs) */
// @retail 0x140420
bool unicode_character_is_private_use(word character)
{
	bool result = false;

	if (character >= 0xe000 && character <= 0xf8ff)
	{
		result = true;
	}

	return result;
}

/* converts UTF-8 to unicode; malformed sequences are skipped */
// @retail 0x140440
void utf8_string_to_utf16_string(const char *source, word *destination, long destination_count)
{
	const byte *bytes = (const byte *)source;
	long source_index = 0;
	long destination_index = 0;

	while (source[source_index])
	{
		dword value;
		long length;
		bool valid = true;
		dword lead = bytes[source_index];

		if (!(lead & 0x80))
		{
			value = lead;
			length = 1;
		}
		else if ((lead & 0xe0) == 0xc0)
		{
			value = lead & ~0xe0u;
			length = 2;
		}
		else if ((lead & 0xf0) == 0xe0)
		{
			value = lead & ~0xf0u;
			length = 3;
		}
		else if ((lead & 0xf8) == 0xf0)
		{
			value = lead & ~0xf8u;
			length = 4;
		}
		else
		{
			valid = false;
		}

		if (valid)
		{
			for (long i = 1; valid && i < length; i++)
			{
				dword continuation = bytes[source_index + i];

				value = (value << 6) | (continuation & ~0xc0u);
				if ((continuation & 0xc0) != 0x80)
				{
					valid = false;
				}
			}
		}

		if (valid)
		{
			source_index += length;
		}
		else
		{
			source_index++;
		}

		if (valid && destination_index < destination_count)
		{
			if (value <= (dword)NONE)
			{
				destination[destination_index] = (word)value;
			}
			else
			{
				destination[destination_index] = '?';
			}
			destination_index++;
		}
	}

	if (destination_index < destination_count)
	{
		destination[destination_index] = 0;
	}
	else if (destination_index > 0)
	{
		destination[destination_index - 1] = 0;
	}
}

/* the UTF-8 encoding of value into buffer; returns its length in bytes */
// @retail 0x1405a0
long utf8_encode_character(dword value, byte *buffer, long buffer_size)
{
	long length;
	dword first_byte;

	if (value <= 0x7f)
	{
		first_byte = value;
		length = 1;
	}
	else if (value <= 0x7ff)
	{
		first_byte = (value | 0x3000) >> 6;
		length = 2;
	}
	else if (value <= 0xffff)
	{
		first_byte = (value | 0xe0000) >> 12;
		length = 3;
	}
	else if (value <= 0x1fffff)
	{
		first_byte = (value | 0x3c00000) >> 18;
		length = 4;
	}
	else
	{
		length = 0;
	}

	if (length > 0 && buffer_size > 0)
	{
		buffer[0] = (byte)first_byte;
	}

	for (long i = 1; i < buffer_size && i < length; i++)
	{
		buffer[i] = (byte)(((value >> (6 * (length - i) - 6)) & 0x3f) | 0x80);
	}

	return length;
}
