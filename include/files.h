/* FILES.H: file references and file operations (src/files.cpp) */

#ifndef FILES_H
#define FILES_H

#include "unknown_11c920.h"
#include <xtl.h>
#include <stddef.h>

#define FILE_REFERENCE_SIGNATURE 0x66696c6f

enum e_file_reference_flags
{
	_file_reference_flag_is_file = 1 << 0 /* clear for a directory */
};

/* the flags of function_136970 (open) */
enum e_file_open_flags
{
	_file_open_flag_read = 1 << 0,
	_file_open_flag_write = 1 << 1,
	_file_open_flag_append = 1 << 2, /* starts at the end of the file */
	_file_open_flag_share_read = 1 << 3, /* lets others read while it is open for writing */
	_file_open_flag_silent = 1 << 4, /* a failure is not reported */
	_file_open_flag_temporary = 1 << 5,
	_file_open_flag_delete_on_close = 1 << 6,
	_file_open_flag_random_access = 1 << 7,
	_file_open_flag_sequential_scan = 1 << 8
};

/* why function_136970 (open) failed */
enum e_file_open_error
{
	_file_open_error_none = 0,
	_file_open_error_file_not_found,
	_file_open_error_access_denied,
	_file_open_error_path_not_found,
	_file_open_error_invalid_drive,
	_file_open_error_sharing_violation,
	_file_open_error_unknown
};

/* a file reference: a path that names a file or a directory, and the open
   file's handle and position */
struct s_type_acf665
{
	dword signature; /* FILE_REFERENCE_SIGNATURE */
	word flags; /* e_file_reference_flags */
	short location;
	char path[256];
	HANDLE handle;
	dword position;
};
typedef char assert_file_reference_size[(sizeof(s_type_acf665) == 0x110) ? 1 : -1];
typedef char assert_file_reference_handle[(offsetof(s_type_acf665, handle) == 0x108) ? 1 : -1];

void function_137320(char *path, const char *name);
void function_1373c0(char *path);
s_type_acf665 *file_ref_create(s_type_acf665 *file, bool directory, const char *name);
bool function_1367d0(s_type_acf665 *file);
bool file_ref_delete(s_type_acf665 *file);
bool function_1368f0(s_type_acf665 *file);
bool function_136970(s_type_acf665 *file, dword flags, dword *error);
bool function_136bb0(s_type_acf665 *file);
bool file_ref_seek(s_type_acf665 *file, dword position, bool silent);
bool file_ref_truncate(s_type_acf665 *file, dword position);
bool file_ref_read(s_type_acf665 *file, dword size, void *buffer, bool silent);
bool function_136d00(s_type_acf665 *file, const void *buffer, dword size);
bool file_ref_read_at(s_type_acf665 *file, dword position, dword size, bool silent, void *buffer);
bool file_ref_write_at(s_type_acf665 *file, dword position, dword size, const void *buffer);
bool file_ref_modified_time(s_type_acf665 *file, FILETIME *time);
void file_ref_clock_time(FILETIME *time);
bool file_ref_size(s_type_acf665 *file, dword *size);

#endif
