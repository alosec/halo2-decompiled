// @flags /O2 /Gr
/* FILES.CPP: file references (a path and whether it names a file or a
   directory) and the file operations on them: create, delete, test, open,
   close, seek, read, write and query */

#include "unknown_11c920.h"
#include <xtl.h>
#include "files.h"

#define MAXIMUM_PATH_SIZE 256

void function_1374c0(const char *path, char *dest); /* unknown_136770.cpp: the path a reference names */

/* a reference to a file, or to a directory, by its path */
// @retail 0x136710
s_type_acf665 *file_ref_create(s_type_acf665 *file, bool directory, const char *name)
{
	memset(file, 0, sizeof(s_type_acf665));
	file->signature = FILE_REFERENCE_SIGNATURE;
	file->location = NONE;
	if (directory)
	{
		function_137320(file->path, name);
		return file;
	}

	if (file->flags & _file_reference_flag_is_file)
	{
		function_1373c0(file->path);
	}
	function_137320(file->path, name);
	file->flags |= _file_reference_flag_is_file;
	return file;
}

/* creates the file (empty) or the directory a reference names */
// @retail 0x1367d0
bool function_1367d0(s_type_acf665 *file)
{
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	function_1374c0(file->path, path);

	if (file->flags & _file_reference_flag_is_file)
	{
		HANDLE handle = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
			return true;
		}
	}
	else if (CreateDirectoryA(path, NULL))
	{
		return true;
	}

	GetLastError();
	SetLastError(0);
	return false;
}

/* deletes the file or the directory a reference names */
// @retail 0x136860
bool file_ref_delete(s_type_acf665 *file)
{
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	function_1374c0(file->path, path);

	if (file->flags & _file_reference_flag_is_file)
	{
		if (SetFileAttributesA(path, FILE_ATTRIBUTE_NORMAL) && DeleteFileA(path))
		{
			return true;
		}
	}
	else if (RemoveDirectoryA(path))
	{
		return true;
	}

	GetLastError();
	SetLastError(0);
	return false;
}

/* whether the file or directory a reference names exists */
// @retail 0x1368f0
bool function_1368f0(s_type_acf665 *file)
{
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	bool success = false;
	function_1374c0(file->path, path);

	if (GetFileAttributesA(path) != 0xFFFFFFFF)
	{
		success = true;
	}
	else if (GetLastError() != ERROR_FILE_NOT_FOUND && GetLastError() != ERROR_PATH_NOT_FOUND)
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

/* opens the file a reference names (e_file_open_flags); on failure, *error
   says why (e_file_open_error) */
// @retail 0x136970
bool function_136970(s_type_acf665 *file, dword flags, dword *error)
{
	bool success = false;
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	dword access = 0;
	dword share = 0;
	dword attributes = FILE_ATTRIBUTE_NORMAL;

	*error = _file_open_error_none;
	function_1374c0(file->path, path);

	if (flags & _file_open_flag_read)
	{
		access = GENERIC_READ;
	}
	if (flags & _file_open_flag_write)
	{
		access |= GENERIC_WRITE;
	}
	if (!(flags & _file_open_flag_write) || (flags & _file_open_flag_share_read))
	{
		share = FILE_SHARE_READ;
	}
	if (flags & _file_open_flag_temporary)
	{
		attributes = FILE_ATTRIBUTE_TEMPORARY;
	}
	if (flags & _file_open_flag_delete_on_close)
	{
		attributes = FILE_FLAG_DELETE_ON_CLOSE;
	}
	if (flags & _file_open_flag_random_access)
	{
		attributes = FILE_FLAG_RANDOM_ACCESS;
	}
	if (flags & _file_open_flag_sequential_scan)
	{
		attributes = FILE_FLAG_SEQUENTIAL_SCAN;
	}

	HANDLE handle = CreateFileA(path, access, share, NULL, OPEN_EXISTING, attributes, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		switch (GetLastError())
		{
		case ERROR_FILE_NOT_FOUND: *error = _file_open_error_file_not_found; break;
		case ERROR_PATH_NOT_FOUND: *error = _file_open_error_path_not_found; break;
		case ERROR_INVALID_DRIVE: *error = _file_open_error_invalid_drive; break;
		case ERROR_ACCESS_DENIED: *error = _file_open_error_access_denied; break;
		case ERROR_SHARING_VIOLATION: *error = _file_open_error_sharing_violation; break;
		case ERROR_INVALID_NAME: *error = _file_open_error_unknown; break;
		default: *error = _file_open_error_unknown; break;
		}
	}
	else
	{
		file->handle = handle;
		file->position = 0;
		success = true;
		if (flags & _file_open_flag_append)
		{
			file->position = SetFilePointer(handle, 0, NULL, FILE_END);
			if (file->position == INVALID_SET_FILE_POINTER)
			{
				CloseHandle(file->handle);
				file->handle = 0;
				file->position = 0;
				success = false;
			}
		}
	}

	if (!success && !(flags & _file_open_flag_silent))
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

/* closes a reference's open file */
// @retail 0x136bb0
bool function_136bb0(s_type_acf665 *file)
{
	bool success = false;
	if (CloseHandle(file->handle))
	{
		file->handle = 0;
		file->position = 0;
		success = true;
	}
	else
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

/* moves an open file's position from its start */
// @retail 0x136bf0
bool file_ref_seek(s_type_acf665 *file, dword position, bool silent)
{
	if (file->position == position)
	{
		return true;
	}
	dword result = SetFilePointer(file->handle, position, NULL, FILE_BEGIN);
	bool success = result != INVALID_SET_FILE_POINTER;
	file->position = result;
	if (!success && !silent)
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

/* ends an open file at a position */
// @retail 0x136c40
bool file_ref_truncate(s_type_acf665 *file, dword position)
{
	bool result;
	bool success = true;
	if (file->position != position)
	{
		dword result = SetFilePointer(file->handle, position, NULL, FILE_BEGIN);
		file->position = result;
		success = result != INVALID_SET_FILE_POINTER;
		if (!success)
		{
			GetLastError();
			SetLastError(0);
		}
	}
	if (success && SetEndOfFile(file->handle))
	{
		result = true;
	}
	else
	{
		result = false;
		GetLastError();
		SetLastError(0);
	}
	return result;
}

/* reads from an open file's position */
// @retail 0x136ca0
bool file_ref_read(s_type_acf665 *file, dword size, void *buffer, bool silent)
{
	dword bytes_read;
	bool success = false;
	if (ReadFile(file->handle, buffer, size, &bytes_read, NULL))
	{
		if (bytes_read == size)
		{
			success = true;
		}
		else
		{
			SetLastError(ERROR_HANDLE_EOF);
		}
	}
	file->position += bytes_read;
	if (!success && !silent)
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

/* writes at an open file's position */
// @retail 0x136d00
bool function_136d00(s_type_acf665 *file, dword size, const void *buffer)
{
	dword bytes_written;
	bool success = false;
	BOOL result = WriteFile(file->handle, buffer, size, &bytes_written, NULL);
	if (result && bytes_written == size)
	{
		success = true;
	}
	file->position += bytes_written;
	if (!success)
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

/* reads from a position in an open file */
#pragma inline_depth(0)
// @retail 0x136d50
bool file_ref_read_at(s_type_acf665 *file, dword position, dword size, bool silent, void *buffer)
{
	return file_ref_seek(file, position, silent) && file_ref_read(file, size, buffer, silent);
}
#pragma inline_depth(255)

/* writes at a position in an open file */
// @retail 0x136d90
bool file_ref_write_at(s_type_acf665 *file, dword position, dword size, const void *buffer)
{
	bool success = true;
	if (file->position != position)
	{
		file->position = SetFilePointer(file->handle, position, NULL, FILE_BEGIN);
		success = file->position != INVALID_SET_FILE_POINTER;
		if (!success)
		{
			GetLastError();
			SetLastError(0);
		}
	}
	return success && function_136d00(file, size, buffer);
}

/* when the file or directory a reference names was last written */
// @retail 0x136df0
bool file_ref_modified_time(s_type_acf665 *file, FILETIME *time)
{
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	WIN32_FILE_ATTRIBUTE_DATA data;

	memset(time, 0, sizeof(FILETIME));
	function_1374c0(file->path, path);
	if (GetFileAttributesExA(path, GetFileExInfoStandard, &data))
	{
		time->dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
		time->dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;
		return true;
	}

	GetLastError();
	SetLastError(0);
	return false;
}

/* the system time, as a file time */
// @retail 0x136e70
void file_ref_clock_time(FILETIME *time)
{
	FILETIME now;
	GetSystemTimeAsFileTime(&now);
	*time = now;
}

/* the size of the file a reference names */
// @retail 0x136e90
bool file_ref_size(s_type_acf665 *file, dword *size)
{
	bool result = false;
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	WIN32_FILE_ATTRIBUTE_DATA data;

	function_1374c0(file->path, path);
	if (GetFileAttributesExA(path, GetFileExInfoStandard, &data))
	{
		*size = data.nFileSizeLow;
		result = true;
	}
	else
	{
		GetLastError();
		SetLastError(0);
	}
	return result;
}
