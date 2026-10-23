// @flags /O2 /GL-
// Havok (third-party physics library): the name of a file without the
// folders in its path, whichever separator they use.
#include "string_helpers_2d8c90.h"

class path_2d7f70
{
public:
	char const *function_2d7f70(char const *path) const;
};

// @retail 0x2d7f70
char const *path_2d7f70::function_2d7f70(char const *path) const
{
	for (int i = 0; i < 2; i++)
	{
		char const *separator = function_2d8da0(path, "/\\"[i]);
		if (separator)
		{
			path = separator + 1;
		}
	}
	return path;
}
