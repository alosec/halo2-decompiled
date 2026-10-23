// @flags /O2 /GL-
// Havok (third-party physics library): writing a string, a number or
// formatted text to the stream beneath a stream, through a buffer on the
// stack.
#include "stream_holder_2d8e70.h"
#include "string_helpers_2d8c90.h"

// the stream beneath: its slot 2 writes characters
class writer_2d8f10 : public referenced_2d88a0
{
public:
	virtual void slot_1() {}
	virtual void write(char const *text, int length) {}
};

// @retail 0x2d8f10
stream_holder_2d8e70 *stream_holder_2d8e70::function_2d8f10(char const *text)
{
	writer_2d8f10 *writer = (writer_2d8f10 *)stream;
	if (text)
	{
		writer->write(text, string_length_2d8d80(text));
	}
	else
	{
		writer->write("(null)", 6);
	}
	return this;
}

// @retail 0x2d8f60
stream_holder_2d8e70 *stream_holder_2d8e70::function_2d8f60(int value)
{
	char buffer[0x400];
	function_2d8ca0(buffer, sizeof(buffer), "%i", value);
	writer_2d8f10 *writer = (writer_2d8f10 *)stream;
	writer->write(buffer, string_length_2d8d80(buffer));
	return this;
}

// @retail 0x2d8fb0
stream_holder_2d8e70 *stream_holder_2d8e70::function_2d8fb0(unsigned int value)
{
	char buffer[0x400];
	function_2d8ca0(buffer, sizeof(buffer), "%u", value);
	writer_2d8f10 *writer = (writer_2d8f10 *)stream;
	writer->write(buffer, string_length_2d8d80(buffer));
	return this;
}

// @retail 0x2d9000
void stream_holder_2d8e70::function_2d9000(char const *format, ...)
{
	char buffer[0x400];
	va_list arguments;
	va_start(arguments, format);
	function_2d8c90(buffer, sizeof(buffer), format, arguments);
	writer_2d8f10 *writer = (writer_2d8f10 *)stream;
	writer->write(buffer, string_length_2d8d80(buffer));
}
