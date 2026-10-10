#include "async.h"
#include <string.h>

#ifdef H2_NATIVE_PORT

/* Exact recovered asynchronous positioned-read submission and worker callback.
 * The callback preserves bounded chunking and delegates file I/O to the real
 * platform boundary rather than fabricating completion or resident bytes. */

extern dword g_46e488;

// @retail 0x1a0e70
long __stdcall function_1a0e70(s_async_task *task)
{
	s_read_position_task *read = &task->read_position;
	LONG high = 0;
	DWORD bytes_read = 0;
	dword const remaining = read->size - read->bytes_read;
	dword const size = remaining > g_46e488 ? g_46e488 : remaining;
	if (SetFilePointer(read->file.handle, read->offset, &high, FILE_BEGIN) !=
		INVALID_SET_FILE_POINTER)
	{
		ReadFile(read->file.handle, (byte *)read->buffer + read->bytes_read,
			size, &bytes_read, NULL);
		read->bytes_read += bytes_read;
		read->offset += bytes_read;
		if (read->bytes_read_out)
			*read->bytes_read_out = read->bytes_read;
	}
	if (read->size == read->bytes_read || bytes_read < size)
		return 1;
	return 0;
}

// @retail 0x1a0f10
long function_1a0f10(s_file_handle file, void *buffer, dword size,
	dword offset, long category, long priority, dword *bytes_read,
	bool volatile *done)
{
	s_async_task task;
	memset(&task, 0, sizeof(task));
	task.read_position.file = file;
	task.read_position.buffer = buffer;
	task.read_position.size = size;
	task.read_position.offset = offset;
	task.read_position.bytes_read_out = bytes_read;
	return function_120ba0(priority, &task, category, function_1a0e70, done);
}

#endif
