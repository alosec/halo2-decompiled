// @flags /O2 /Gr
/* FILE_REFERENCE_PATH.CPP: the path of a file reference, as the asynchronous
   file helpers (unknown_1a08d0.cpp) open it. Moved here from
   unknown_1a1870.cpp. */

#include "unknown_11c920.h"

/* unknown_136770.cpp's file reference */
struct file_reference_data
{
	dword signature;
	byte flags; /* e_file_reference_flags: bit 0 set for a file, clear for a directory */
	byte unknown05;
	word unknown06;
	char path[256];
	byte unknown108[8];
};

void function_1374c0(const char *path, char *dest);
void function_137400(char *path, char **a, char **b, char **c, char **d, bool flag);
void function_137320(char *path, const char *name);
void function_137370(char *path, const char *extension);

/* the file's path: its directory, name and extension */
// @retail 0x1a1870
char *file_reference_get_path(file_reference_data const *file, char *path)
{
	char parsed_path[256] = "";
	char *name;
	char *extension;
	char *directory;
	char *parent;

	function_1374c0(file->path, parsed_path);
	function_137400(parsed_path, &name, &extension, &directory, &parent, (bool)(file->flags & 1));
	path[0] = 0;
	function_137320(path, directory);
	function_137320(path, name);
	function_137370(path, extension);
	return path;
}
