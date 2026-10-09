// @flags /O2 /Ob1 /Gr
/* UNKNOWN_216A50.CPP: saved game file types and sizes in blocks on the
   saved game drive (the user interface's block counts call these) */

#include "unknown_11c920.h"
#include <xtl.h>
#include "screen_widgets.h"

char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);

/* the saved game drive: saved games go to the utility drive when set */
struct s_saved_game_drive
{
	byte utility_drive : 1;
	byte unused : 7;
};

s_saved_game_drive g_4e61c0;

/* the saved game file type of a game variant's game engine */
// @retail 0x216a50
long saved_game_file_type_from_variant(s_game_variant *variant)
{
	switch (variant->field_xcb8724)
	{
	case 1:
		return 7;
	case 9:
		return 8;
	case 2:
		return 1;
	case 3:
		return 4;
	case 4:
		return 2;
	case 8:
		return 9;
	case 7:
		return 5;
	default:
		__assume(0);
	}
}

static inline void saved_game_drive_root(char *root, long maximum_count)
{
	char letter = TEST_FIELD_BIT(g_4e61c0.utility_drive) ? 'u' : 0;

	if (TEST_FIELD_BIT(g_4e61c0.utility_drive))
	{
#ifdef H2_NATIVE_PORT
		/* The native virtual drive has the same one-letter root syntax.  Avoid
		   retaining the full recovered printf engine for this fixed string. */
		if (maximum_count > 0)
			root[0] = 0;
		if (maximum_count > 1)
		{
			root[0] = letter;
			root[1] = 0;
		}
		if (maximum_count > 2)
		{
			root[1] = ':';
			root[2] = 0;
		}
		if (maximum_count > 3)
		{
			root[2] = '\\';
			root[3] = 0;
		}
#else
		function_11c9c0(root, maximum_count, "%c:\\", letter);
#endif
	}
	else
	{
		root[0] = 0;
	}
}

/* the blocks a file of this size takes on the saved game drive (a block is
   16 KB; a file takes six clusters more for its directory entries) */
// @retail 0x216c70
long saved_game_file_size_in_blocks(long size)
{
	char root[8];

	saved_game_drive_root(root, sizeof(root));
	return (size + (long)XGetDiskClusterSize(root) * 6 + 0x3fff) / 0x4000;
}

/* the blocks a saved game file of this type takes: type 0 is a player
   profile, types 1..9 game variants, the rest playlists */
// @retail 0x216c40
long saved_game_file_type_size_in_blocks(long type)
{
	long size;

	if (type)
	{
		size = type > 9 ? 0x15cb8 : 0x130;
	}
	else
	{
		size = 0x1e0;
	}
	return saved_game_file_size_in_blocks(size);
}

/* whether the saved game drive has this many blocks free */
// @retail 0x216ce0
bool saved_game_storage_has_free_blocks(long blocks)
{
	__int64 bytes = blocks << 14;
	char root[8];
	ULARGE_INTEGER available;
	ULARGE_INTEGER total;
	ULARGE_INTEGER free;

	saved_game_drive_root(root, sizeof(root));
	if (GetDiskFreeSpaceExA(root, &available, &total, &free) && available.QuadPart >= (unsigned __int64)bytes)
	{
		return true;
	}
	return false;
}
