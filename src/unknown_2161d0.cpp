// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_2accd0.h"
#include <string.h>

struct s_profile_file_location
{
	char name[0x14];
	wchar_t display_name[0x12];
	long type;
	byte unknown3c;
	char language;
	byte unknown3e[2];
};

struct s_cached_profile_files
{
	long count;
	struct
	{
		long unknown0;
		byte profile[0x1e0];
	} entries[1];
};

extern void *g_51ea14;
extern long g_55c154;

void function_125d60(void);
bool function_216800(long file_index, void *location);
const char *saved_game_type_get_storage_name(long type);
char *function_122810(char *path, const char *suffix);
bool saved_game_file_read_begin(void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task, const char *path);
bool function_2162b0(long file_index, void *buffer, long size, s_saved_game_file_task *task);

// @retail 0x2161d0
bool function_2161d0(long file_index, void *buffer, long size)
{
	bool result = false;
	s_saved_game_file_task task;

	if (function_2162b0(file_index, buffer, size, &task))
	{
		if (!*(volatile bool *)&task.done)
		{
			while (!*(volatile bool *)&task.done)
			{
				SwitchToThread();
				function_125d60();
			}
		}
		result = task.succeeded;
		if (!result)
			g_55c154 = task.state;
	}
	return result;
}

// @retail 0x2162b0
bool function_2162b0(long file_index, void *buffer, long size, s_saved_game_file_task *task)
{
	bool result = false;

	if ((bool)(((dword)file_index >> 21) & 1) && g_51ea14)
	{
		s_cached_profile_files *files = (s_cached_profile_files *)g_51ea14;
		long index = (file_index >> 8) & 0x1fff;
		long bounded_index = index < 0 ? 0 : index > files->count - 1 ? files->count - 1 : index;
		if (bounded_index == index && (dword)size <= 0x1e0)
		{
			memcpy(buffer, files->entries[index].profile, size);
			task->done = true;
			task->succeeded = true;
			task->state = 0;
			task->progress = 1.0f;
			result = true;
		}
	}
	else
	{
		struct
		{
			s_profile_file_location field_0;
			byte field_40[4];
			char field_44[0x100];
		} local_0;
		local_0.field_0.name[0] = 0;
		local_0.field_0.display_name[0] = 0;
		local_0.field_44[0] = 0;
		if (function_216800(file_index, &local_0.field_0) && local_0.field_0.language != -1)
		{
			long type = local_0.field_0.type;
			strncpy(local_0.field_44, local_0.field_0.name, sizeof(local_0.field_44));
			local_0.field_44[sizeof(local_0.field_44) - 1] = 0;
			function_122810(local_0.field_44, saved_game_type_get_storage_name(type));
			result = saved_game_file_read_begin(buffer, size, false, task, local_0.field_44);
		}
	}
	return result;
}
