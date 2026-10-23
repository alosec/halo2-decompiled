// @flags /O2 /GL-
// Havok (third-party physics library): copies an object's text into a buffer
// of a given size, and gives the text's length.
#include "string_helpers_2d8c90.h"

class text_2d95a0
{
public:
	char unknown0[8];
	char const *text;

	unsigned int function_2d95a0(char *buffer, unsigned int size) const;
};

// @retail 0x2d95a0
unsigned int text_2d95a0::function_2d95a0(char *buffer, unsigned int size) const
{
	function_2d8d60(buffer, text, size);
	return string_length_2d8d80(text);
}
