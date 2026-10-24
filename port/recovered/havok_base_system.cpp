// @flags /O2 /GL- /GX
// Havok (third-party physics library): the base system (0x2d8780 starts it,
// 0x2d7f00 stops it) and what it builds. Starting installs four globals
// (each swap drops a reference to what it replaces):
//   g_480118  the memory, the game's (unknown_1470f0.cpp)
//   g_480180  the stream factory (vtable 0x411d2c): console writers and
//             C-library file readers and writers
//   g_48017c  the console streams (vtable 0x411d20): an output stream and an
//             error stream over console writers
//   g_480178  the error handler (vtable 0x411da4): formats messages, and
//             keeps the ids and source locations whose messages are
//             suppressed
// and then creates the singletons on the list at g_480174. Static
// initialisers (0x378ad0-0x379670, run from the CRT's table in retail, and
// by h2_run_retail_initializers in the port) put
// eight nodes on that list; each node names a create function and the global
// that holds the singleton. A create function that returns null is retried
// after the others, until all exist. Stopping releases the singletons in
// reverse order of creation, then the four globals.
//
// Objects are made with `new T`, not `new T()`: for a class without a
// constructor of its own, the parentheses zero the object after the class's
// operator new has recorded its size.
#include <stdio.h>
#include <stdarg.h>
#include <new>
#include "unknown_11c920.h"
#include "reference_counted_2d88a0.h"
#include "reference_base_318970.h"
#include "growable_array_2d91d0.h"
#include "stream_holder_2d8e70.h"
#include "hash_table_2d9240.h"
#include "havok_reference.h"
#include "unknown_146a20.h"

extern "C"
{
	// the C library's file calls the file streams use (retail links libcmt's)
	__SIZE_TYPE__ fread(void *buffer, __SIZE_TYPE__ size, __SIZE_TYPE__ count, FILE *stream);
	__SIZE_TYPE__ fwrite(void const *buffer, __SIZE_TYPE__ size, __SIZE_TYPE__ count, FILE *stream);
	int fseek(FILE *stream, long offset, int origin);
	long ftell(FILE *stream);
	int fflush(FILE *stream);
	int vsprintf(char *buffer, char const *format, va_list arguments);
	// the XDK's debugger output
	void __stdcall OutputDebugStringA(char const *text);
}

// ---------------------------------------------------------------------------
// recovered code this file calls, declared as src declares it

// the game's Havok memory (unknown_1470f0.cpp)
c_havok_abstract_allocator *function_1473b0(void);
// the shared empty function (unknown_157450.cpp)
void function_175f40();

// a deleting destructor with nothing of its own to destroy, tag 0x13
// (unknown_2da270.cpp); the classes whose slot 0 is 0x2da270 derive from it
class class_2da270 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x13)
};

// an output stream's constructor over a writer, which it holds a reference
// to (unknown_2d9090.cpp); the object is the one stream_holder_2d8e70.h
// declares (vtable 0x411e2c, destroyed by 0x2d8e70)
class holder_2d9090
{
public:
	long field_4;
	referenced_2d88a0 *field_8;

	holder_2d9090(referenced_2d88a0 *object);
	virtual ~holder_2d9090() {}
};

// writing one character to an output stream (unknown_2d8ef0.cpp)
class class_2d8ef0_stream
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual int write(const void *data, int size) = 0;
};

class class_2d8ef0
{
public:
	void *vtable;
	long field_4;
	class_2d8ef0_stream *stream;

	class_2d8ef0 *append_character(char value);
};

// flushing an output stream (unknown_2d9070.cpp)
class class_2d9070_stream
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void flush() = 0;
};

struct class_2d9070
{
	void *vtable;
	long field_4;
	class_2d9070_stream *stream;
};

class_2d9070 *__cdecl function_2d9070(class_2d9070 *writer);

// a reader over another with a buffer of its own (unknown_2d9c00.cpp)
class buffered_2d9c00 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x15)

	referenced_2d88a0 *source;
	void *buffer;
	long field_10;
	long field_14;
	long size;
	long field_1c;
	long field_20;

	buffered_2d9c00(referenced_2d88a0 *source_, long size_);
	~buffered_2d9c00();
};

// a reader's skip, through its read slot (unknown_2d9c00.cpp)
class reader_2d9c80 : public referenced_2d88a0
{
public:
	virtual void slot_1() {}
	virtual long read(void *buffer, long size) { return 0; }
	long skip(long count);
};

// a writer over another with a buffer of its own (unknown_2d9e40.cpp)
class buffered_writer_2d9e40 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x15)

	referenced_2d88a0 *stream;
	void *buffer;
	long size;
	long capacity;
	bool owns_buffer;

	buffered_writer_2d9e40(referenced_2d88a0 *stream_, long capacity_);
	~buffered_writer_2d9e40();
};

// a returned hkBool: a class, so the result goes through a hidden pointer as
// retail's does (src declares its virtual hkBool slots this way)
struct hkbool_2d7fe0
{
	char value;
	hkbool_2d7fe0() {}
	hkbool_2d7fe0(bool value) : value(value) {}
};

// stream slots that are one-line functions shared with other classes
// as src/unknown_2fe820.cpp declares it (its name is part of the symbol)
struct hkbool_2fe820
{
	char value;
	hkbool_2fe820() {}
};

struct class_2fe820
{
	hkbool_2fe820 function_2fe820();
};

struct class_2d9840
{
	long function_2d9840(long arg_0, long arg_1);
};

// a path's file name without its folders (unknown_2d7f70.cpp)
class path_2d7f70
{
public:
	char const *function_2d7f70(char const *path) const;
};

// the pointer table's constructor, its release, and its walk
// (unknown_2d9470.cpp, unknown_2d94b0.cpp, hash_table_2d9240.h): one retail
// class under three names
class table_2d9470
{
public:
	void *entries;
	long field_4;
	long mask;

	table_2d9470();
};

class class_2d94b0
{
public:
	void *slots;
	long field_4;
	long mask;

	void release();
};

// a shared string's header, 12 bytes before its characters; 0x2d8e50 gives
// it back (unknown_2d8e10.cpp)
class class_2d8e50
{
public:
	long field_0;
	long length;

	void release();
};

// the singletons' create functions that src recovers
class scratch_owner_2d88a0;
class empty_2da2a0;
class array_owner_2dc150;
scratch_owner_2d88a0 *__cdecl function_2d8990();
empty_2da2a0 *__cdecl function_2da2a0();
array_owner_2dc150 *__cdecl function_2dc150();

// ---------------------------------------------------------------------------
// small functions of the library

// a one-line slot shared by several stream classes: "yes"
struct true_31bc80
{
	hkbool_2d7fe0 function_31bc80();
};

// @retail 0x31bc80
hkbool_2d7fe0 true_31bc80::function_31bc80()
{
	return hkbool_2d7fe0(true);
}

// formats into a buffer with no size limit
// @retail 0x2d8cc0
int __cdecl function_2d8cc0(char *buffer, char const *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	return vsprintf(buffer, format, arguments);
}

// compares two strings ignoring the case of ASCII letters: below zero, zero
// or above zero as the first sorts before, with or after the second
// @retail 0x2d8ce0
int __cdecl function_2d8ce0(char const *first, char const *second)
{
	for (long i = 0;; i++)
	{
		char a = first[i];
		char b = second[i];
		if (a == 0 && b == 0)
		{
			return 0;
		}
		char lower_a = a >= 'A' && a <= 'Z' ? a + 0x20 : a;
		char lower_b = b >= 'A' && b <= 'Z' ? b + 0x20 : b;
		if (lower_a < lower_b)
		{
			return -1;
		}
		if (lower_a > lower_b)
		{
			return 1;
		}
	}
}

// ---------------------------------------------------------------------------
// the stream factory and its streams

// a writer to the console: the debugger's output and the C library's
// (vtable 0x411d3c)
class console_writer : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x15)

	// slot 0 is 0x2d7720: the base's destructor (0x318970), then the block
	// back with tag 0x15
	virtual ~console_writer() {}
	virtual hkbool_2d7fe0 is_ok()
	{
		return ((true_31bc80 *)this)->function_31bc80();
	}
	virtual long write(void const *data, long size);
	virtual void flush()
	{
		function_175f40();
	}
	virtual hkbool_2d7fe0 seek_tell_supported()
	{
		return hkbool_2d7fe0(((class_2fe820 *)this)->function_2fe820().value != 0);
	}
	// fails (1)
	virtual long seek(long offset, long origin)
	{
		return ((class_2d9840 *)this)->function_2d9840(offset, origin);
	}
	// the same code as 0x224240, which src recovers as a particle interface
	// slot
	virtual long tell()
	{
		return NONE;
	}
};
static_assert(sizeof(console_writer) == 8, "console writer layout");

// writes the text, copied with a terminator when it has none
// @retail 0x2d7c00
long console_writer::write(void const *data, long size)
{
	char const *text = (char const *)data;
	if (size == 0)
	{
		return size;
	}
	if (text[size - 1] == 0)
	{
		OutputDebugStringA(text);
		printf("%s", text);
		return size;
	}
	// a growable array of size + 1 characters
	long capacity = size + 1;
	char *copy = (char *)g_memory_2d88a0->allocate_block(capacity, 0x12);
	for (long i = 0; i < size; i++)
	{
		copy[i] = text[i];
	}
	copy[size] = 0;
	OutputDebugStringA(copy);
	printf("%s", copy);
	if (capacity >= 0)
	{
		g_memory_2d88a0->release_block(copy, capacity & 0x7fffffff, 0x12);
	}
	return size;
}

// a reader of a file opened through the C library (vtable 0x411d5c); it can
// mark a position and seek back to it
class file_reader : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x15)

	long mark;
	FILE *file;
	bool ok;

	file_reader(char const *name);
	// slot 0 is 0x2d7960, the deleting destructor of 0x2d7990
	virtual ~file_reader();
	virtual hkbool_2d7fe0 is_ok();
	virtual long read(void *buffer, long size);
	virtual long skip(long count)
	{
		return ((reader_2d9c80 *)this)->skip(count);
	}
	virtual hkbool_2d7fe0 mark_supported()
	{
		return ((true_31bc80 *)this)->function_31bc80();
	}
	virtual long set_mark(long limit);
	virtual long rewind_to_mark();
	virtual hkbool_2d7fe0 seek_tell_supported()
	{
		return ((true_31bc80 *)this)->function_31bc80();
	}
	virtual long seek(long offset, long origin);
	virtual long tell();
};
static_assert(sizeof(file_reader) == 0x14, "file reader layout");

// @retail 0x2d7880
file_reader::file_reader(char const *name)
{
	mark = NONE;
	ok = true;
	file = fopen(name, "rb");
	ok = file != 0;
}

// @retail 0x2d7990
file_reader::~file_reader()
{
	if (file)
	{
		fclose(file);
	}
}

// @retail 0x2d7920
hkbool_2d7fe0 file_reader::is_ok()
{
	return hkbool_2d7fe0(ok);
}

// a read that gets nothing leaves the reader no longer ok
// @retail 0x2d78f0
long file_reader::read(void *buffer, long size)
{
	long count = (long)fread(buffer, 1, size, file);
	if (count <= 0)
	{
		ok = false;
	}
	return count;
}

// marks the current position; fails (1) when it is unknown
// @retail 0x2d78c0
long file_reader::set_mark(long limit)
{
	mark = tell();
	return mark == NONE;
}

// @retail 0x2d78e0
long file_reader::rewind_to_mark()
{
	return seek(mark, 0);
}

// 0 when it worked, 1 when it failed
// @retail 0x2d7930
long file_reader::seek(long offset, long origin)
{
	return fseek(file, offset, origin) != 0;
}

// @retail 0x2d7950
long file_reader::tell()
{
	return ftell(file);
}

// a writer to a file opened through the C library (vtable 0x411d88)
class file_writer : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x15)

	FILE *file;
	bool owns_file;

	file_writer(char const *name);
	// slot 0 is 0x2d7ba0, the deleting destructor of 0x2d7bd0
	virtual ~file_writer();
	virtual hkbool_2d7fe0 is_ok();
	virtual long write(void const *data, long size);
	virtual void flush();
	virtual hkbool_2d7fe0 seek_tell_supported()
	{
		return ((true_31bc80 *)this)->function_31bc80();
	}
	virtual long seek(long offset, long origin);
	virtual long tell();

	void close();
};
static_assert(sizeof(file_writer) == 0x10, "file writer layout");

// @retail 0x2d7aa0
file_writer::file_writer(char const *name)
{
	owns_file = true;
	file = fopen(name, "wb");
}

// @retail 0x2d7bd0
file_writer::~file_writer()
{
	if (file && owns_file)
	{
		fclose(file);
	}
	file = 0;
}

// @retail 0x2d7b50
hkbool_2d7fe0 file_writer::is_ok()
{
	return hkbool_2d7fe0(file != 0);
}

// a write that puts nothing out closes the file
// @retail 0x2d7ad0
long file_writer::write(void const *data, long size)
{
	if (!file)
	{
		return 0;
	}
	long count = (long)fwrite(data, 1, size, file);
	if (count <= 0)
	{
		close();
	}
	return count;
}

// @retail 0x2d7b40
void file_writer::flush()
{
	if (file)
	{
		fflush(file);
	}
}

// 0 when it worked, 1 when it failed
// @retail 0x2d7b70
long file_writer::seek(long offset, long origin)
{
	return fseek(file, offset, origin) != 0;
}

// @retail 0x2d7b90
long file_writer::tell()
{
	return ftell(file);
}

// @retail 0x2d7b10
void file_writer::close()
{
	if (file && owns_file)
	{
		fclose(file);
	}
	file = 0;
}

// what makes streams (vtable 0x411d2c, 8 bytes, g_480180): slot 0 is
// 0x2da270
class stream_factory : public class_2da270
{
public:
	virtual referenced_2d88a0 *open_console(long which);
	virtual referenced_2d88a0 *open_reader(char const *name);
	virtual referenced_2d88a0 *open_writer(char const *name);
};
static_assert(sizeof(stream_factory) == 8, "stream factory layout");

// the same writer whichever console is asked for
// @retail 0x2d76f0
referenced_2d88a0 *stream_factory::open_console(long which)
{
	return new console_writer;
}

// a file reader, behind a 4 KB buffer when it cannot mark positions itself
// @retail 0x2d7750
referenced_2d88a0 *stream_factory::open_reader(char const *name)
{
	file_reader *reader = new file_reader(name);
	if (!reader->mark_supported().value)
	{
		buffered_2d9c00 *buffered = new buffered_2d9c00(reader, 0x1000);
		havok_reference_remove((c_havok_reference_counted *)reader);
		return buffered;
	}
	return reader;
}

// a file writer behind a 4 KB buffer
// @retail 0x2d79c0
referenced_2d88a0 *stream_factory::open_writer(char const *name)
{
	file_writer *writer = new file_writer(name);
	buffered_writer_2d9e40 *buffered = new buffered_writer_2d9e40(writer, 0x1000);
	havok_reference_remove((c_havok_reference_counted *)writer);
	return buffered;
}

extern stream_factory *g_480180;

// the output stream and the error stream (vtable 0x411d20, 0x20 bytes,
// g_48017c), each over a console writer. The streams are built in place by
// holder_2d9090's constructor (0x2d9090) and destroyed by
// stream_holder_2d8e70's destructor (0x2d8e70): one retail class, which src
// declares under both names.
class console_streams : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x13)

	long output_storage[3];
	long error_storage[3];

	console_streams();
	// slot 0 is 0x2d7670, the deleting destructor of 0x2d76a0
	virtual ~console_streams();
	virtual stream_holder_2d8e70 *output();
	virtual stream_holder_2d8e70 *error_output();
};
static_assert(sizeof(console_streams) == 0x20, "console streams layout");

// each stream holds the only reference to its writer once built
// @retail 0x2d7570
console_streams::console_streams()
{
	new (output_storage) holder_2d9090(g_480180->open_console(1));
	new (error_storage) holder_2d9090(g_480180->open_console(2));
	havok_reference_remove((c_havok_reference_counted *)output()->stream);
	havok_reference_remove((c_havok_reference_counted *)error_output()->stream);
}

// @retail 0x2d76a0
console_streams::~console_streams()
{
	error_output()->stream_holder_2d8e70::~stream_holder_2d8e70();
	output()->stream_holder_2d8e70::~stream_holder_2d8e70();
}

// @retail 0x2d7650
stream_holder_2d8e70 *console_streams::output()
{
	return (stream_holder_2d8e70 *)output_storage;
}

// @retail 0x2d7660
stream_holder_2d8e70 *console_streams::error_output()
{
	return (stream_holder_2d8e70 *)error_storage;
}

extern console_streams *g_48017c;

// ---------------------------------------------------------------------------
// the error handler

// a source location whose messages are suppressed, and how many times
struct error_location
{
	char const *file;
	long line;
	long count;
};

struct error_locations
{
	ELEMENTS_2D91D0_MEMBERS(error_location)

	long *find(error_location const &key);
	long *get(error_location const &key);
	long remove(error_location const &key);
};

// a message id and its state (present means suppressed)
struct error_id
{
	long id;
	long value;
};

struct error_ids
{
	ELEMENTS_2D91D0_MEMBERS(error_id)

	long remove(long const &key);
	long *get(long const &key);
};

// the count of a location (same line, file names equal ignoring case), or
// null
// @retail 0x2d8280
long *error_locations::find(error_location const &key)
{
	for (long i = 0; i < size; i++)
	{
		if (data[i].line == key.line && function_2d8ce0(data[i].file, key.file) == 0)
		{
			return &data[i].count;
		}
	}
	return 0;
}

// the count of a location, the location added (count not set) when absent
// @retail 0x2d85a0
long *error_locations::get(error_location const &key)
{
	for (long i = 0; i < size; i++)
	{
		if (data[i].line == key.line && function_2d8ce0(data[i].file, key.file) == 0)
		{
			return &data[i].count;
		}
	}
	long index = size;
	ensure_capacity(index + 1);
	size = index + 1;
	data[index].file = key.file;
	data[index].line = key.line;
	return &data[index].count;
}

// 0 when the location was there and is removed, 1 when it was absent
// @retail 0x2d8700
long error_locations::remove(error_location const &key)
{
	for (long i = size - 1; i >= 0; i--)
	{
		if (data[i].line == key.line && function_2d8ce0(data[i].file, key.file) == 0)
		{
			remove_at(i);
			return 0;
		}
	}
	return 1;
}

// 0 when the id was there and is removed, 1 when it was absent
// @retail 0x2d8410
long error_ids::remove(long const &key)
{
	for (long i = size - 1; i >= 0; i--)
	{
		if (data[i].id == key)
		{
			remove_at(i);
			return 0;
		}
	}
	return 1;
}

// the value of an id, the id added (value not set) when absent
// @retail 0x2d8460
long *error_ids::get(long const &key)
{
	for (long i = 0; i < size; i++)
	{
		if (data[i].id == key)
		{
			return &data[i].value;
		}
	}
	long index = size;
	ensure_capacity(index + 1);
	size = index + 1;
	data[index].id = key;
	return &data[index].value;
}

// the error handler (vtable 0x411da4, 0x20 bytes, g_480178). Its messages
// are an error, a warning and a report, each with a title, an id, a
// description and the source location; suppressed ids and locations are
// dropped, the rest are formatted and handed to an output function that the
// retail build leaves empty.
class error_handler : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x13)

	error_locations locations;
	error_ids suppressed_ids;

	error_handler()
	{
		locations.data = 0;
		locations.size = 0;
		locations.capacity_and_flags = 0x80000000;
		suppressed_ids.data = 0;
		suppressed_ids.size = 0;
		suppressed_ids.capacity_and_flags = 0x80000000;
	}

	// slot 0 is 0x2d84e0, the deleting destructor of 0x2d8510
	virtual ~error_handler();
	virtual void warning(char const *title, long id, char const *description, char const *file, long line);
	virtual void error(char const *title, long id, char const *description, char const *file, long line);
	virtual void report(char const *title, long id, char const *description, char const *file, long line);
	virtual void set_enabled(long id, bool enabled);
	virtual hkbool_2d7fe0 is_enabled(long id);
	// slots 6 to 8 share one function, as do 9 to 11
	virtual void suppress_at(char const *file, long line);
	virtual void suppress_at_7(char const *file, long line)
	{
		error_handler::suppress_at(file, line);
	}
	virtual void suppress_at_8(char const *file, long line)
	{
		error_handler::suppress_at(file, line);
	}
	virtual void unsuppress_at(char const *file, long line);
	virtual void unsuppress_at_10(char const *file, long line)
	{
		error_handler::unsuppress_at(file, line);
	}
	virtual void unsuppress_at_11(char const *file, long line)
	{
		error_handler::unsuppress_at(file, line);
	}
	virtual void show_message(char const *text);
	virtual char const *file_name(char const *path)
	{
		return ((path_2d7f70 *)this)->function_2d7f70(path);
	}

	void is_suppressed(bool *result, char const *file, long line, long id);
	void output(long type, char const *title, long id, char const *description, char const *file,
		long line, bool flag);
};
static_assert(sizeof(error_handler) == 0x20, "error handler layout");

// @retail 0x2d8510
error_handler::~error_handler()
{
	if (suppressed_ids.capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(suppressed_ids.data,
			(suppressed_ids.capacity_and_flags & 0x7fffffff) * sizeof(error_id), 0x12);
	}
	if (locations.capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(locations.data,
			(locations.capacity_and_flags & 0x7fffffff) * sizeof(error_location), 0x12);
	}
}

// whether the location (by file name, without folders) or the id is
// suppressed
// @retail 0x2d81f0
void error_handler::is_suppressed(bool *result, char const *file, long line, long id)
{
	error_location key;
	key.file = file_name(file);
	key.line = line;
	if (locations.find(key))
	{
		*result = true;
		return;
	}
	for (long i = 0; i < suppressed_ids.size; i++)
	{
		if (suppressed_ids[i].id == id)
		{
			*result = true;
			return;
		}
	}
	*result = false;
}

// formats "file(line): [0xid] title : 'description'" and a new line; the
// retail build hands the text to an empty function for every type (0 an
// error, 2 a warning, 3 a report) and never reads the flag
// @retail 0x2d8080
void error_handler::output(long type, char const *title, long id, char const *description,
	char const *file, long line, bool flag)
{
	char id_text[12];
	function_2d8cc0(id_text, "0x%x", id);
	char buffer[0x200];
	stream_holder_2d8e70 stream(buffer, sizeof(buffer), flag_2d8e70(true));
	stream.function_2d8f10(file);
	((class_2d8ef0 *)&stream)->append_character('(');
	stream.function_2d8f60(line)
		->function_2d8f10("): [")
		->function_2d8f10(id_text)
		->function_2d8f10("] ")
		->function_2d8f10(title)
		->function_2d8f10(" : '")
		->function_2d8f10(description)
		->function_2d8f10("'\n");
	function_175f40();
}

// @retail 0x2d8030
void error_handler::warning(char const *title, long id, char const *description, char const *file, long line)
{
	bool suppressed;
	is_suppressed(&suppressed, file, line, id);
	if (!suppressed)
	{
		output(2, title, id, description, file, line, true);
	}
}

// @retail 0x2d82e0
void error_handler::error(char const *title, long id, char const *description, char const *file, long line)
{
	bool suppressed;
	is_suppressed(&suppressed, file, line, id);
	if (!suppressed)
	{
		output(0, title, id, description, file, line, true);
	}
}

// @retail 0x2d8330
void error_handler::report(char const *title, long id, char const *description, char const *file, long line)
{
	bool suppressed;
	is_suppressed(&suppressed, file, line, id);
	if (!suppressed)
	{
		output(3, title, id, description, file, line, false);
	}
}

// @retail 0x2d8380
void error_handler::set_enabled(long id, bool enabled)
{
	long found = 0;
	for (long i = 0; i < suppressed_ids.size; i++)
	{
		if (suppressed_ids[i].id == id)
		{
			found = 1;
			break;
		}
	}
	if (enabled)
	{
		if (found)
		{
			suppressed_ids.remove(id);
		}
	}
	else if (!found)
	{
		*suppressed_ids.get(id) = 1;
	}
}

// @retail 0x2d7fe0
hkbool_2d7fe0 error_handler::is_enabled(long id)
{
	for (long i = 0; i < suppressed_ids.size; i++)
	{
		if (suppressed_ids[i].id == id)
		{
			return hkbool_2d7fe0(false);
		}
	}
	return hkbool_2d7fe0(true);
}

// @retail 0x2d8640
void error_handler::suppress_at(char const *file, long line)
{
	error_location key;
	key.file = file_name(file);
	key.line = line;
	if (!locations.find(key))
	{
		*locations.get(key) = 1;
	}
	else
	{
		(*locations.get(key))++;
	}
}

// the location goes once its count was already 0 before this call
// @retail 0x2d86a0
void error_handler::unsuppress_at(char const *file, long line)
{
	error_location key;
	key.file = file_name(file);
	key.line = line;
	if (locations.find(key))
	{
		long *count = locations.get(key);
		if ((*count)-- == 0)
		{
			locations.remove(key);
		}
	}
}

// writes the text to the error stream and flushes it
// @retail 0x2d7fb0
void error_handler::show_message(char const *text)
{
	function_2d9070((class_2d9070 *)g_48017c->error_output()->function_2d8f10(text));
}

extern error_handler *g_480178;

// ---------------------------------------------------------------------------
// the singletons

// the names given to objects (vtable 0x411d14, 0x14 bytes, g_48011c): a
// pointer table from each object to a copy of its name. Its two methods
// (0x2d7120, 0x2d7180), which unknown_2d7210.cpp declares, are not
// recovered here.
class c_physics_registration_manager : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x13)

	table_2d9470 names;

	c_physics_registration_manager();
	// slot 0 is 0x2d72c0, the deleting destructor of 0x2d72f0
	virtual ~c_physics_registration_manager();
};
static_assert(sizeof(c_physics_registration_manager) == 0x14, "registration manager layout");

c_physics_registration_manager *g_48011c;

// @retail 0x2d7270
c_physics_registration_manager::c_physics_registration_manager()
{
}

// gives back every name, then the table
// @retail 0x2d72f0
c_physics_registration_manager::~c_physics_registration_manager()
{
	class_2d9240 *table = (class_2d9240 *)&names;
	for (int index = table->first(); table->is_valid(index).value; index = table->next(index))
	{
		g_memory_2d88a0->release((void *)table->word_at(index));
	}
	((class_2d94b0 *)&names)->release();
}

// @retail 0x2d73a0
c_physics_registration_manager *__cdecl function_2d73a0()
{
	return new c_physics_registration_manager;
}

// what the base system starts last (vtable 0x411d18, 8 bytes, g_480148):
// slot 0 is 0x2da270, and its start slot is the empty function
class startup_hook : public class_2da270
{
public:
	virtual void start()
	{
		function_175f40();
	}
};
static_assert(sizeof(startup_hook) == 8, "startup hook layout");

startup_hook *g_480148;

// @retail 0x2d7f40
startup_hook *__cdecl function_2d7f40()
{
	return new startup_hook;
}

// an object holding a 16 KB buffer and a cursor into it (vtable 0x43e508,
// 0x4020 bytes, g_484b38): slot 0 is 0x2da270
class buffer_singleton : public class_2da270
{
public:
	char *cursor;
	long capacity;
	bool flag;
	char unknown11[0xf];
	char buffer[0x4000];

	buffer_singleton()
	{
		cursor = buffer;
		capacity = sizeof(buffer);
		flag = false;
	}
};
static_assert(sizeof(buffer_singleton) == 0x4020, "buffer singleton layout");

buffer_singleton *g_484b38;

// @retail 0x3724b0
buffer_singleton *__cdecl function_3724b0()
{
	return new buffer_singleton;
}

// an object holding an array of four-byte entries (vtable 0x43e528, 0x14
// bytes, tag 0xc, g_484b40)
class array_singleton : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0xc)

	array_2d91d0 entries;

	array_singleton()
	{
		entries.data = 0;
		entries.size = 0;
		entries.capacity_and_flags = 0x80000000;
	}
	// slot 0 is 0x372730, the deleting destructor of 0x372680
	virtual ~array_singleton();
};
static_assert(sizeof(array_singleton) == 0x14, "array singleton layout");

array_singleton *g_484b40;

// @retail 0x372680
array_singleton::~array_singleton()
{
	if (entries.capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(entries.data, (entries.capacity_and_flags & 0x7fffffff) * 4, 0x12);
	}
}

// @retail 0x3726f0
array_singleton *__cdecl function_3726f0()
{
	return new array_singleton;
}

// a shared string's header, 12 bytes before its characters
struct shared_string_header
{
	long field_0;
	long length;
	long references;
};

// an entry that starts with a shared string
struct named_entry
{
	char *name;
	long field_4;
	long field_8;
};

struct named_entries
{
	ELEMENTS_2D91D0_MEMBERS(named_entry)
};

// an object holding a field and an array of named entries (vtable 0x43e52c,
// 0x18 bytes, g_484b44)
class named_entries_singleton : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x13)

	long field_8;
	named_entries entries;

	named_entries_singleton()
	{
		field_8 = 0;
		entries.data = 0;
		entries.size = 0;
		entries.capacity_and_flags = 0x80000000;
	}
	// slot 0 is 0x3727d0, the deleting destructor of 0x372800
	virtual ~named_entries_singleton();
};
static_assert(sizeof(named_entries_singleton) == 0x18, "named entries singleton layout");

named_entries_singleton *g_484b44;

// drops each entry's string (given back with its last reference), then the
// array
// @retail 0x372800
named_entries_singleton::~named_entries_singleton()
{
	for (long i = 0; i < entries.size; i++)
	{
		shared_string_header *header = (shared_string_header *)(entries[i].name - sizeof(shared_string_header));
		if (--header->references < 0)
		{
			((class_2d8e50 *)header)->release();
		}
	}
	if (entries.capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(entries.data,
			(entries.capacity_and_flags & 0x7fffffff) * sizeof(named_entry), 0x12);
	}
}

// @retail 0x372790
named_entries_singleton *__cdecl function_372790()
{
	return new named_entries_singleton;
}

// the singletons whose create functions src recovers
void *g_480160; // scratch_owner_2d88a0 (0x2d8990)
void *g_4801d4; // empty_2da2a0 (0x2da2a0)
void *g_4802a4; // array_owner_2dc150 (0x2dc150)

// ---------------------------------------------------------------------------
// the singleton list

typedef void *(__cdecl *singleton_create)();

// a singleton on the list: what creates it, the next node, and the global
// that holds it (a reference-counted object, or null before it exists)
struct singleton_node
{
	singleton_create create;
	singleton_node *next;
	void **instance;
};

ELEMENTS_2D91D0(singleton_nodes, singleton_node *);

singleton_node *g_480174;
bool g_48014c;

// the nodes (retail keeps them at these addresses)
singleton_node g_461de4 = { (singleton_create)function_2d73a0, 0, (void **)&g_48011c };
singleton_node g_461df8 = { (singleton_create)function_2d7f40, 0, (void **)&g_480148 };
singleton_node g_461e04 = { (singleton_create)function_2d8990, 0, &g_480160 };
singleton_node g_461e7c = { (singleton_create)function_2da2a0, 0, &g_4801d4 };
singleton_node g_461ee0 = { (singleton_create)function_2dc150, 0, &g_4802a4 };
singleton_node g_466dd8 = { (singleton_create)function_3724b0, 0, (void **)&g_484b38 };
singleton_node g_466de4 = { (singleton_create)function_3726f0, 0, (void **)&g_484b40 };
singleton_node g_466df0 = { (singleton_create)function_372790, 0, (void **)&g_484b44 };

// each static initialiser puts one node at the head of the list
// @retail 0x378ad0
void __cdecl function_378ad0()
{
	g_461de4.next = g_480174;
	g_480174 = &g_461de4;
}

// @retail 0x378af0
void __cdecl function_378af0()
{
	g_461df8.next = g_480174;
	g_480174 = &g_461df8;
}

// @retail 0x378b10
void __cdecl function_378b10()
{
	g_461e04.next = g_480174;
	g_480174 = &g_461e04;
}

// @retail 0x378ba0
void __cdecl function_378ba0()
{
	g_461e7c.next = g_480174;
	g_480174 = &g_461e7c;
}

// @retail 0x378c00
void __cdecl function_378c00()
{
	g_461ee0.next = g_480174;
	g_480174 = &g_461ee0;
}

// @retail 0x379630
void __cdecl function_379630()
{
	g_466dd8.next = g_480174;
	g_480174 = &g_466dd8;
}

// @retail 0x379650
void __cdecl function_379650()
{
	g_466de4.next = g_480174;
	g_480174 = &g_466de4;
}

// @retail 0x379670
void __cdecl function_379670()
{
	g_466df0.next = g_480174;
	g_480174 = &g_466df0;
}

// retail runs these initialisers from the CRT's table (0x46119c-0x4612a4);
// the port's h2_run_retail_initializers (retail_initializers_generated.cpp)
// calls them, in the table's order, before main()

// creates every singleton not yet created. A node whose create function
// returns null leaves the list and is retried, last first, until all are
// created; each node is put back at the end of the list as its singleton is
// created, so the list ends up in the order of creation.
// @retail 0x2d7ca0
void __cdecl function_2d7ca0()
{
	singleton_nodes deferred;
	deferred.data = 0;
	deferred.size = 0;
	deferred.capacity_and_flags = 0x80000000;
	singleton_node **link = &g_480174;
	singleton_node *node = g_480174;
	while (node)
	{
		if (*node->instance == 0)
		{
			void *instance = node->create();
			if (!instance)
			{
				deferred.append(node);
				node = node->next;
				(*link)->next = 0;
				*link = node;
				continue;
			}
			*node->instance = instance;
		}
		link = &node->next;
		node = *link;
	}
	while (deferred.size != 0)
	{
		for (long i = deferred.size - 1; i >= 0; i--)
		{
			singleton_node *waiting = deferred[i];
			void *instance = waiting->create();
			if (instance)
			{
				*waiting->instance = instance;
				*link = waiting;
				deferred.remove_at(i);
				link = &waiting->next;
			}
		}
	}
	if (deferred.capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(deferred.data, (deferred.capacity_and_flags & 0x7fffffff) * 4, 0x12);
	}
}

// drops every singleton's reference, last created first, and clears its
// global
// @retail 0x2d7dd0
void __cdecl function_2d7dd0()
{
	singleton_nodes nodes;
	nodes.data = 0;
	nodes.size = 0;
	nodes.capacity_and_flags = 0x80000000;
	function_2d9160(&nodes, 0x10, sizeof(singleton_node *));
	for (singleton_node *node = g_480174; node; node = node->next)
	{
		nodes.append(node);
	}
	for (long i = nodes.size - 1; i >= 0; i--)
	{
		havok_reference_remove((c_havok_reference_counted *)*nodes[i]->instance);
		*nodes[i]->instance = 0;
	}
	if (nodes.capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(nodes.data, (nodes.capacity_and_flags & 0x7fffffff) * 4, 0x12);
	}
}

// ---------------------------------------------------------------------------
// installing the globals and starting and stopping the base system

// the memory keeps its reference count in a long at +4; it goes with its
// last reference (cache-file memory is no concern here). The new memory's
// count is not raised.
// @retail 0x2d6fe0
void __cdecl function_2d6fe0(c_havok_abstract_allocator *memory)
{
	c_havok_abstract_allocator *old = (c_havok_abstract_allocator *)g_480118;
	if (old)
	{
		old->m_unknown04--;
		if (old->m_unknown04 == 0)
		{
			delete old;
		}
	}
	g_480118 = (c_allocator *)memory;
}

// the error handler; the old one loses a reference
// @retail 0x2d73f0
void __cdecl function_2d73f0(error_handler *handler)
{
	if (g_480178)
	{
		havok_reference_remove((c_havok_reference_counted *)g_480178);
	}
	g_480178 = handler;
}

// the console streams; the old ones lose a reference
// @retail 0x2d7460
void __cdecl function_2d7460(console_streams *streams)
{
	if (g_48017c)
	{
		havok_reference_remove((c_havok_reference_counted *)g_48017c);
	}
	g_48017c = streams;
}

// the stream factory; the old one loses a reference
// @retail 0x2d74d0
void __cdecl function_2d74d0(stream_factory *factory)
{
	if (g_480180)
	{
		havok_reference_remove((c_havok_reference_counted *)g_480180);
	}
	g_480180 = factory;
}

stream_factory *g_480180;

// starts the base system once: the memory, the stream factory, the console
// streams (which need the factory), the error handler, then the singletons
// (retail returns 0, success)
// @retail 0x2d8780
void __cdecl function_2d8780()
{
	if (!g_48014c)
	{
		function_2d6fe0(function_1473b0());
		function_2d74d0(new stream_factory);
		function_2d7460(new console_streams);
		function_2d73f0(new error_handler);
		function_2d7ca0();
		g_480148->start();
		g_48014c = true;
	}
}

// stops it: the singletons, then the globals in reverse order (retail
// returns 0)
// @retail 0x2d7f00
void __cdecl function_2d7f00()
{
	if (g_48014c == true)
	{
		function_2d7dd0();
		function_2d73f0(0);
		function_2d7460(0);
		function_2d74d0(0);
		function_2d6fe0(0);
		g_48014c = false;
	}
}
