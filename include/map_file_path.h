/* MAP_FILE_PATH.H: the file path a map name stands for (0x214db0) and a
   buffer that path is printed into */

#ifndef MAP_FILE_PATH_H
#define MAP_FILE_PATH_H

void map_file_path_get(char const *map_name, char *path);

/* a buffer a map name's file path is printed into */
struct s_map_file_path
{
	char text[256];
	void get(char const *map_name) { map_file_path_get(map_name, text); }
};

#endif
