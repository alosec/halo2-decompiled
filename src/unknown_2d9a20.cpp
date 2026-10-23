// @flags /O2 /GL-
// Havok (third-party physics library): a buffered stream over another
// stream.

struct bool_2d9a20
{
	bool value;

	bool_2d9a20(bool value_)
	{
		value = value_;
	}
};

class class_2d9a20_stream
{
public:
	virtual void slot0() = 0;
	virtual bool_2d9a20 slot1() = 0;
	virtual int slot2(void *data, int size) = 0;
	virtual void slot3() = 0;
	virtual void slot4() = 0;
	virtual void slot5() = 0;
	virtual void slot6() = 0;
	virtual bool_2d9a20 slot7() = 0;
	virtual void slot8() = 0;
	virtual int slot9() = 0;
};

class class_2d9a20
{
public:
	void *vtable;
	long field_4;
	class_2d9a20_stream *stream;
	char *buffer;
	int position;
	int end;
	int field_18;
	int field_1c;
	int field_20;

	bool_2d9a20 function_2d9a20();
	bool_2d9a20 function_2d9a60();
	int function_2d9a80(int value);
	int function_2d9aa0();
	bool_2d9a20 function_2d9ac0();
	void function_2d9b00();
	int function_2d9b20();
};

// @retail 0x2d9a20
bool_2d9a20 class_2d9a20::function_2d9a20()
{
	return bool_2d9a20(position != end || stream->slot1().value);
}

// @retail 0x2d9a60
bool_2d9a20 class_2d9a20::function_2d9a60()
{
	return bool_2d9a20(field_18 != 0);
}

// @retail 0x2d9a80
int class_2d9a20::function_2d9a80(int value)
{
	field_1c = position;
	field_20 = value;
	return value > field_18;
}

// @retail 0x2d9aa0
int class_2d9a20::function_2d9aa0()
{
	if (field_1c >= 0)
	{
		position = field_1c;
		return 0;
	}
	return 1;
}

// @retail 0x2d9ac0
bool_2d9a20 class_2d9a20::function_2d9ac0()
{
	return bool_2d9a20(stream && stream->slot7().value);
}

// @retail 0x2d9b00
void class_2d9a20::function_2d9b00()
{
	field_1c = -1;
	field_20 = -1;
	position = 0;
	end = 0;
	stream->slot8();
}

// @retail 0x2d9b20
int class_2d9a20::function_2d9b20()
{
	int buffered = end - position;
	int offset = stream->slot9();
	if (offset >= 0)
	{
		return offset - buffered;
	}
	return -1;
}
